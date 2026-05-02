#include "protocol/TelemetryPacket.hpp"
#include "protocol/Crc32.hpp"
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <limits>
#include <sstream>

namespace satgcs::protocol {
namespace {

template <class T>
void put_le(std::array<std::byte, TelemetryPacket::WireSize>& buffer, std::size_t& offset, T value) noexcept {
    static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
    using U = std::make_unsigned_t<T>;
    const auto unsignedValue = static_cast<U>(value);
    for (std::size_t i = 0U; i < sizeof(T); ++i) {
        buffer.at(offset) = static_cast<std::byte>((unsignedValue >> (8U * i)) & static_cast<U>(0xffU));
        ++offset;
    }
}

template <class T>
T get_le(std::span<const std::byte> buffer, std::size_t& offset) noexcept {
    static_assert(std::is_integral_v<T> || std::is_enum_v<T>);
    using U = std::make_unsigned_t<T>;
    U value = 0U;
    for (std::size_t i = 0U; i < sizeof(T); ++i) {
        value = static_cast<U>(value | (static_cast<U>(std::to_integer<std::uint8_t>(buffer[offset])) << (8U * i)));
        ++offset;
    }
    return static_cast<T>(value);
}

bool is_valid_status(std::uint8_t value) noexcept {
    return value <= static_cast<std::uint8_t>(Status::Critical);
}

std::string escape_json(const std::string& value) {
    std::ostringstream out;
    for (const char c : value) {
        switch (c) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default: out << c; break;
        }
    }
    return out.str();
}

std::string sanitize_satellite_id(const std::string& source) {
    const auto maxSize = TelemetryPacket::SatelliteIdWireSize;
    return source.substr(0U, maxSize);
}

} // namespace

std::int64_t nowUnixMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

const char* statusName(Status status) noexcept {
    switch (status) {
        case Status::Normal: return "NORMAL";
        case Status::Warning: return "WARNING";
        case Status::Critical: return "CRITICAL";
    }
    return "UNKNOWN";
}

const char* decodeErrorName(DecodeError error) noexcept {
    switch (error) {
        case DecodeError::None: return "NONE";
        case DecodeError::InvalidSize: return "INVALID_SIZE";
        case DecodeError::InvalidCrc: return "INVALID_CRC";
        case DecodeError::InvalidMagic: return "INVALID_MAGIC";
        case DecodeError::UnsupportedVersion: return "UNSUPPORTED_VERSION";
        case DecodeError::InvalidStatus: return "INVALID_STATUS";
    }
    return "UNKNOWN";
}

std::array<std::byte, TelemetryPacket::WireSize> TelemetryPacket::encode() const {
    std::array<std::byte, WireSize> buffer{};
    std::size_t offset = 0U;

    put_le<std::uint16_t>(buffer, offset, Magic);
    put_le<std::uint8_t>(buffer, offset, Version);
    put_le<std::uint8_t>(buffer, offset, 0U); // reserved flags
    put_le<std::uint32_t>(buffer, offset, sequence);

    std::array<char, SatelliteIdWireSize> id{};
    const auto cleanId = sanitize_satellite_id(satelliteId);
    std::memcpy(id.data(), cleanId.data(), cleanId.size());
    for (const char c : id) {
        buffer.at(offset) = static_cast<std::byte>(static_cast<unsigned char>(c));
        ++offset;
    }

    put_le<std::int64_t>(buffer, offset, unixMs);
    put_le<std::uint16_t>(buffer, offset, batteryMv);
    put_le<std::int16_t>(buffer, offset, temperatureCentiC);
    put_le<std::int32_t>(buffer, offset, rollMdeg);
    put_le<std::int32_t>(buffer, offset, pitchMdeg);
    put_le<std::int32_t>(buffer, offset, yawMdeg);
    put_le<std::int32_t>(buffer, offset, latE7);
    put_le<std::int32_t>(buffer, offset, lonE7);
    put_le<std::uint8_t>(buffer, offset, static_cast<std::uint8_t>(status));
    put_le<std::uint8_t>(buffer, offset, 0U);
    put_le<std::uint8_t>(buffer, offset, 0U);
    put_le<std::uint8_t>(buffer, offset, 0U);

    const auto checksum = crc32(std::span<const std::byte>(buffer.data(), WireSize - sizeof(std::uint32_t)));
    put_le<std::uint32_t>(buffer, offset, checksum);
    return buffer;
}

DecodeResult TelemetryPacket::decodeDetailed(std::span<const std::byte> bytes) noexcept {
    if (bytes.size() != WireSize) {
        return {.packet = std::nullopt, .error = DecodeError::InvalidSize};
    }

    const auto expectedCrc = crc32(bytes.first(WireSize - sizeof(std::uint32_t)));
    std::size_t crcOffset = WireSize - sizeof(std::uint32_t);
    const auto actualCrc = get_le<std::uint32_t>(bytes, crcOffset);
    if (expectedCrc != actualCrc) {
        return {.packet = std::nullopt, .error = DecodeError::InvalidCrc};
    }

    std::size_t offset = 0U;
    if (get_le<std::uint16_t>(bytes, offset) != Magic) {
        return {.packet = std::nullopt, .error = DecodeError::InvalidMagic};
    }
    if (get_le<std::uint8_t>(bytes, offset) != Version) {
        return {.packet = std::nullopt, .error = DecodeError::UnsupportedVersion};
    }

    static_cast<void>(get_le<std::uint8_t>(bytes, offset)); // flags reserved

    TelemetryPacket packet{};
    packet.sequence = get_le<std::uint32_t>(bytes, offset);

    std::array<char, SatelliteIdWireSize + 1U> id{};
    for (std::size_t i = 0U; i < SatelliteIdWireSize; ++i) {
        id[i] = static_cast<char>(std::to_integer<std::uint8_t>(bytes[offset]));
        ++offset;
    }
    packet.satelliteId = std::string(id.data());
    packet.unixMs = get_le<std::int64_t>(bytes, offset);
    packet.batteryMv = get_le<std::uint16_t>(bytes, offset);
    packet.temperatureCentiC = get_le<std::int16_t>(bytes, offset);
    packet.rollMdeg = get_le<std::int32_t>(bytes, offset);
    packet.pitchMdeg = get_le<std::int32_t>(bytes, offset);
    packet.yawMdeg = get_le<std::int32_t>(bytes, offset);
    packet.latE7 = get_le<std::int32_t>(bytes, offset);
    packet.lonE7 = get_le<std::int32_t>(bytes, offset);

    const auto rawStatus = get_le<std::uint8_t>(bytes, offset);
    if (!is_valid_status(rawStatus)) {
        return {.packet = std::nullopt, .error = DecodeError::InvalidStatus};
    }
    packet.status = static_cast<Status>(rawStatus);

    return {.packet = packet, .error = DecodeError::None};
}

std::optional<TelemetryPacket> TelemetryPacket::decode(std::span<const std::byte> bytes) noexcept {
    return decodeDetailed(bytes).packet;
}

std::string TelemetryPacket::toJson() const {
    std::ostringstream out;
    out << "{"
        << "\"satelliteId\":\"" << escape_json(satelliteId) << "\","
        << "\"sequence\":" << sequence << ","
        << "\"timestampMs\":" << unixMs << ","
        << "\"batteryVoltage\":" << std::fixed << std::setprecision(3) << (static_cast<double>(batteryMv) / 1000.0) << ","
        << "\"temperature\":" << std::fixed << std::setprecision(2) << (static_cast<double>(temperatureCentiC) / 100.0) << ","
        << "\"roll\":" << std::fixed << std::setprecision(3) << (static_cast<double>(rollMdeg) / 1000.0) << ","
        << "\"pitch\":" << std::fixed << std::setprecision(3) << (static_cast<double>(pitchMdeg) / 1000.0) << ","
        << "\"yaw\":" << std::fixed << std::setprecision(3) << (static_cast<double>(yawMdeg) / 1000.0) << ","
        << "\"lat\":" << std::fixed << std::setprecision(7) << (static_cast<double>(latE7) / 10000000.0) << ","
        << "\"lon\":" << std::fixed << std::setprecision(7) << (static_cast<double>(lonE7) / 10000000.0) << ","
        << "\"status\":\"" << statusName(status) << "\"}"
        ;
    return out.str();
}

} // namespace satgcs::protocol
