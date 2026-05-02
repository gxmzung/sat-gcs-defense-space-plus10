#include "util/Logger.hpp"
#include <atomic>
#include <chrono>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <sstream>

namespace satgcs::util {
namespace {
std::atomic<Level> g_minimumLevel{Level::Info};
std::mutex& logMutex() {
    static std::mutex m;
    return m;
}

const char* levelName(Level level) noexcept {
    switch (level) {
        case Level::Debug: return "DEBUG";
        case Level::Info: return "INFO";
        case Level::Warn: return "WARN";
        case Level::Error: return "ERROR";
    }
    return "UNKNOWN";
}

std::string escapeJson(std::string_view input) {
    std::ostringstream out;
    for (const char c : input) {
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

std::string utcTimestamp() {
    using namespace std::chrono;
    const auto now = system_clock::now();
    const auto tt = system_clock::to_time_t(now);
    const auto ms = duration_cast<milliseconds>(now.time_since_epoch()).count() % 1000;
    std::tm tm{};
    gmtime_r(&tt, &tm);
    std::ostringstream out;
    out << std::put_time(&tm, "%FT%T") << '.' << std::setw(3) << std::setfill('0') << ms << 'Z';
    return out.str();
}

bool shouldLog(Level level) noexcept {
    return static_cast<std::uint8_t>(level) >= static_cast<std::uint8_t>(g_minimumLevel.load());
}
} // namespace

void Logger::setMinimumLevel(Level level) noexcept {
    g_minimumLevel.store(level);
}

void Logger::log(Level level, std::string_view component, std::string_view message) {
    if (!shouldLog(level)) {
        return;
    }
    std::lock_guard<std::mutex> lock(logMutex());
    std::cerr << "{\"ts\":\"" << utcTimestamp()
              << "\",\"level\":\"" << levelName(level)
              << "\",\"component\":\"" << escapeJson(component)
              << "\",\"message\":\"" << escapeJson(message) << "\"}\n";
}

void Logger::logKv(Level level,
                   std::string_view component,
                   std::string_view event,
                   std::string_view key,
                   std::string_view value) {
    if (!shouldLog(level)) {
        return;
    }
    std::lock_guard<std::mutex> lock(logMutex());
    std::cerr << "{\"ts\":\"" << utcTimestamp()
              << "\",\"level\":\"" << levelName(level)
              << "\",\"component\":\"" << escapeJson(component)
              << "\",\"event\":\"" << escapeJson(event)
              << "\",\"" << escapeJson(key) << "\":\"" << escapeJson(value) << "\"}\n";
}

void Logger::logKv(Level level,
                   std::string_view component,
                   std::string_view event,
                   std::string_view key,
                   std::int64_t value) {
    if (!shouldLog(level)) {
        return;
    }
    std::lock_guard<std::mutex> lock(logMutex());
    std::cerr << "{\"ts\":\"" << utcTimestamp()
              << "\",\"level\":\"" << levelName(level)
              << "\",\"component\":\"" << escapeJson(component)
              << "\",\"event\":\"" << escapeJson(event)
              << "\",\"" << escapeJson(key) << "\":" << value << "}\n";
}

} // namespace satgcs::util
