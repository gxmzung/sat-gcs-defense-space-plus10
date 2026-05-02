#include "protocol/Crc32.hpp"
namespace satgcs::protocol {
uint32_t crc32(std::span<const std::byte> bytes) noexcept {
    uint32_t crc = 0xFFFFFFFFu;
    for (auto b : bytes) {
        crc ^= static_cast<uint8_t>(b);
        for (int i = 0; i < 8; ++i) {
            uint32_t mask = -(crc & 1u);
            crc = (crc >> 1) ^ (0xEDB88320u & mask);
        }
    }
    return ~crc;
}
}
