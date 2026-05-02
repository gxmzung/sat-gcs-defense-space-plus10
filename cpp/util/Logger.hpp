#pragma once
#include <cstdint>
#include <string>
#include <string_view>

namespace satgcs::util {

enum class Level : std::uint8_t { Debug = 0U, Info = 1U, Warn = 2U, Error = 3U };

class Logger final {
public:
    static void setMinimumLevel(Level level) noexcept;
    static void log(Level level, std::string_view component, std::string_view message);
    static void logKv(Level level,
                      std::string_view component,
                      std::string_view event,
                      std::string_view key,
                      std::string_view value);
    static void logKv(Level level,
                      std::string_view component,
                      std::string_view event,
                      std::string_view key,
                      std::int64_t value);
};

} // namespace satgcs::util
