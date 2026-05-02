#pragma once
#include <array>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace satgcs::protocol {

// Traceability: REQ-GCS-002, REQ-GCS-003, REQ-GCS-004, REQ-GCS-005
// Wire protocol uses fixed-width integer types and explicit validation results.

enum class Status : std::uint8_t { Normal = 0U, Warning = 1U, Critical = 2U };

enum class DecodeError : std::uint8_t {
    None = 0U,
    InvalidSize,
    InvalidCrc,
    InvalidMagic,
    UnsupportedVersion,
    InvalidStatus
};

struct DecodeResult;

struct TelemetryPacket final {
    static constexpr std::uint16_t Magic = 0x5347U;
    static constexpr std::uint8_t Version = 1U;
    static constexpr std::size_t WireSize = 64U;
    static constexpr std::size_t SatelliteIdWireSize = 16U;

    std::uint32_t sequence{};
    std::string satelliteId{"KOMPSAT-SIM-01"};
    std::int64_t unixMs{};
    std::uint16_t batteryMv{};
    std::int16_t temperatureCentiC{};
    std::int32_t rollMdeg{};
    std::int32_t pitchMdeg{};
    std::int32_t yawMdeg{};
    std::int32_t latE7{};
    std::int32_t lonE7{};
    Status status{Status::Normal};

    [[nodiscard]] std::array<std::byte, WireSize> encode() const;
    [[nodiscard]] static DecodeResult decodeDetailed(std::span<const std::byte> bytes) noexcept;
    [[nodiscard]] static std::optional<TelemetryPacket> decode(std::span<const std::byte> bytes) noexcept;
    [[nodiscard]] std::string toJson() const;
};

struct DecodeResult final {
    std::optional<TelemetryPacket> packet{};
    DecodeError error{DecodeError::None};

    [[nodiscard]] bool ok() const noexcept { return packet.has_value(); }
};

[[nodiscard]] std::int64_t nowUnixMs();
[[nodiscard]] const char* statusName(Status s) noexcept;
[[nodiscard]] const char* decodeErrorName(DecodeError e) noexcept;
}
