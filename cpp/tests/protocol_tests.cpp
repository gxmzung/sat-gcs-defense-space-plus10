#include "protocol/Crc32.hpp"
#include "protocol/TelemetryPacket.hpp"
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <vector>

using namespace satgcs::protocol;

namespace {
void write_crc(std::array<std::byte, TelemetryPacket::WireSize>& wire) {
    const auto crc = crc32(std::span<const std::byte>(wire.data(), wire.size() - sizeof(std::uint32_t)));
    const auto offset = wire.size() - sizeof(std::uint32_t);
    wire[offset + 0U] = static_cast<std::byte>((crc >> 0U) & 0xffU);
    wire[offset + 1U] = static_cast<std::byte>((crc >> 8U) & 0xffU);
    wire[offset + 2U] = static_cast<std::byte>((crc >> 16U) & 0xffU);
    wire[offset + 3U] = static_cast<std::byte>((crc >> 24U) & 0xffU);
}

TelemetryPacket samplePacket() {
    TelemetryPacket packet;
    packet.sequence = 42U;
    packet.unixMs = 123;
    packet.batteryMv = 7400U;
    packet.temperatureCentiC = 3210;
    packet.rollMdeg = 1200;
    packet.pitchMdeg = -400;
    packet.yawMdeg = 80500;
    packet.latE7 = 363500000;
    packet.lonE7 = 1273845000;
    packet.status = Status::Normal;
    return packet;
}

void encode_decode_round_trip() {
    const auto packet = samplePacket();
    const auto wire = packet.encode();
    const auto decoded = TelemetryPacket::decodeDetailed(std::span<const std::byte>(wire.data(), wire.size()));
    assert(decoded.ok());
    assert(decoded.error == DecodeError::None);
    assert(decoded.packet->sequence == 42U);
    assert(decoded.packet->batteryMv == 7400U);
    assert(decoded.packet->temperatureCentiC == 3210);
}

void rejects_invalid_size() {
    const std::vector<std::byte> shortPacket(10U);
    const auto decoded = TelemetryPacket::decodeDetailed(std::span<const std::byte>(shortPacket.data(), shortPacket.size()));
    assert(!decoded.ok());
    assert(decoded.error == DecodeError::InvalidSize);
}

void rejects_corrupted_packet() {
    auto wire = samplePacket().encode();
    wire[10] = std::byte{0xffU};
    const auto decoded = TelemetryPacket::decodeDetailed(std::span<const std::byte>(wire.data(), wire.size()));
    assert(!decoded.ok());
    assert(decoded.error == DecodeError::InvalidCrc);
}

void rejects_invalid_magic_with_valid_crc() {
    auto wire = samplePacket().encode();
    wire[0] = std::byte{0x00U};
    write_crc(wire);
    const auto decoded = TelemetryPacket::decodeDetailed(std::span<const std::byte>(wire.data(), wire.size()));
    assert(!decoded.ok());
    assert(decoded.error == DecodeError::InvalidMagic);
}

void rejects_unsupported_version_with_valid_crc() {
    auto wire = samplePacket().encode();
    wire[2] = std::byte{0x7fU};
    write_crc(wire);
    const auto decoded = TelemetryPacket::decodeDetailed(std::span<const std::byte>(wire.data(), wire.size()));
    assert(!decoded.ok());
    assert(decoded.error == DecodeError::UnsupportedVersion);
}

void rejects_invalid_status_with_valid_crc() {
    auto wire = samplePacket().encode();
    constexpr std::size_t statusOffset = 56U;
    wire[statusOffset] = std::byte{0xffU};
    write_crc(wire);
    const auto decoded = TelemetryPacket::decodeDetailed(std::span<const std::byte>(wire.data(), wire.size()));
    assert(!decoded.ok());
    assert(decoded.error == DecodeError::InvalidStatus);
}
} // namespace

int main() {
    encode_decode_round_trip();
    rejects_invalid_size();
    rejects_corrupted_packet();
    rejects_invalid_magic_with_valid_crc();
    rejects_unsupported_version_with_valid_crc();
    rejects_invalid_status_with_valid_crc();
    std::cout << "protocol_tests passed\n";
    return 0;
}
