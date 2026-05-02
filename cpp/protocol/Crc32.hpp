#pragma once
#include <cstddef>
#include <cstdint>
#include <span>
namespace satgcs::protocol {
uint32_t crc32(std::span<const std::byte> bytes) noexcept;
}
