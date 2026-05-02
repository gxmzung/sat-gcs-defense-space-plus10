#include "protocol/TelemetryPacket.hpp"
#include "util/HttpClient.hpp"
#include "util/Logger.hpp"
#include <arpa/inet.h>
#include <array>
#include <atomic>
#include <chrono>
#include <csignal>
#include <cstdint>
#include <deque>
#include <optional>
#include <string>
#include <thread>
#include <unistd.h>

namespace {
using satgcs::util::Level;

std::atomic<bool> g_running{true};

void handleSignal(int) noexcept {
    g_running.store(false);
}

struct GatewayConfig final {
    std::string listen{"0.0.0.0"};
    std::string server{"http://localhost:8080"};
    std::string apiKey{"dev-api-key"};
    int port{46000};
    int maxQueue{1000};
    int maxDrainPerLoop{5};
    int connectTimeoutMs{1000};
    int readTimeoutMs{2000};
    int retryAttempts{3};
};

std::optional<int> parseInt(const std::string& value) noexcept {
    try {
        return std::stoi(value);
    } catch (...) {
        return std::nullopt;
    }
}

GatewayConfig parseArgs(int argc, char** argv) {
    GatewayConfig config{};
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto next = [&]() -> std::string { return (i + 1 < argc) ? std::string(argv[++i]) : std::string{}; };
        if (arg == "--listen") config.listen = next();
        else if (arg == "--server") config.server = next();
        else if (arg == "--api-key") config.apiKey = next();
        else if (arg == "--port") config.port = parseInt(next()).value_or(config.port);
        else if (arg == "--max-queue") config.maxQueue = parseInt(next()).value_or(config.maxQueue);
        else if (arg == "--connect-timeout-ms") config.connectTimeoutMs = parseInt(next()).value_or(config.connectTimeoutMs);
        else if (arg == "--read-timeout-ms") config.readTimeoutMs = parseInt(next()).value_or(config.readTimeoutMs);
        else if (arg == "--retry-attempts") config.retryAttempts = parseInt(next()).value_or(config.retryAttempts);
    }
    return config;
}

bool postWithRetry(const satgcs::util::HttpClient& client,
                   const GatewayConfig& config,
                   const std::string& json,
                   std::uint32_t sequence) {
    for (int attempt = 1; attempt <= config.retryAttempts; ++attempt) {
        const auto result = client.postJson(config.server, "/api/telemetry", config.apiKey, json);
        if (result.ok) {
            satgcs::util::Logger::logKv(Level::Info, "gateway", "forward_success", "sequence", static_cast<std::int64_t>(sequence));
            return true;
        }
        satgcs::util::Logger::logKv(Level::Warn, "gateway", "forward_failed", "attempt", static_cast<std::int64_t>(attempt));
        satgcs::util::Logger::logKv(Level::Warn, "gateway", "forward_failed", "status_code", static_cast<std::int64_t>(result.statusCode));
        if (attempt < config.retryAttempts) {
            const auto delayMs = 100 * (1 << (attempt - 1));
            std::this_thread::sleep_for(std::chrono::milliseconds(delayMs));
        }
    }
    return false;
}
} // namespace

int main(int argc, char** argv) {
    std::signal(SIGINT, handleSignal);
    std::signal(SIGTERM, handleSignal);

    const auto config = parseArgs(argc, argv);
    satgcs::util::HttpClient client({std::chrono::milliseconds(config.connectTimeoutMs),
                                     std::chrono::milliseconds(config.readTimeoutMs),
                                     80U});

    const int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        satgcs::util::Logger::log(Level::Error, "gateway", "socket_create_failed");
        return 1;
    }

    timeval receiveTimeout{};
    receiveTimeout.tv_sec = 1;
    receiveTimeout.tv_usec = 0;
    setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, &receiveTimeout, sizeof(receiveTimeout));

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(static_cast<std::uint16_t>(config.port));
    addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(fd, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0) {
        satgcs::util::Logger::log(Level::Error, "gateway", "udp_bind_failed");
        close(fd);
        return 1;
    }

    satgcs::util::Logger::logKv(Level::Info, "gateway", "startup", "udp_port", static_cast<std::int64_t>(config.port));
    satgcs::util::Logger::logKv(Level::Info, "gateway", "startup", "server", config.server);

    std::deque<std::pair<std::uint32_t, std::string>> retryQueue;
    std::uint64_t packetsReceived = 0U;
    std::uint64_t packetsRejected = 0U;

    while (g_running.load()) {
        std::array<std::byte, satgcs::protocol::TelemetryPacket::WireSize> buffer{};
        sockaddr_in from{};
        socklen_t len = sizeof(from);
        const auto n = recvfrom(fd, buffer.data(), buffer.size(), 0, reinterpret_cast<sockaddr*>(&from), &len);

        if (n < 0) {
            // receive timeout used to allow graceful shutdown and queue draining.
        } else if (n != static_cast<ssize_t>(buffer.size())) {
            ++packetsRejected;
            satgcs::util::Logger::logKv(Level::Warn, "gateway", "packet_rejected_invalid_size", "size", static_cast<std::int64_t>(n));
        } else {
            ++packetsReceived;
            const auto decoded = satgcs::protocol::TelemetryPacket::decodeDetailed(std::span<const std::byte>(buffer.data(), buffer.size()));
            if (!decoded.ok()) {
                ++packetsRejected;
                satgcs::util::Logger::logKv(Level::Warn, "gateway", "packet_rejected_protocol", "error", satgcs::protocol::decodeErrorName(decoded.error));
            } else {
                const auto& packet = decoded.packet.value();
                const auto json = packet.toJson();
                if (!postWithRetry(client, config, json, packet.sequence)) {
                    if (static_cast<int>(retryQueue.size()) < config.maxQueue) {
                        retryQueue.emplace_back(packet.sequence, json);
                        satgcs::util::Logger::logKv(Level::Warn, "gateway", "queued_for_retry", "queue_size", static_cast<std::int64_t>(retryQueue.size()));
                    } else {
                        satgcs::util::Logger::logKv(Level::Error, "gateway", "retry_queue_full_drop", "sequence", static_cast<std::int64_t>(packet.sequence));
                    }
                }
            }
        }

        int drained = 0;
        while (!retryQueue.empty() && drained < config.maxDrainPerLoop) {
            const auto item = retryQueue.front();
            if (!postWithRetry(client, config, item.second, item.first)) {
                break;
            }
            retryQueue.pop_front();
            ++drained;
        }
    }

    satgcs::util::Logger::logKv(Level::Info, "gateway", "shutdown", "packets_received", static_cast<std::int64_t>(packetsReceived));
    satgcs::util::Logger::logKv(Level::Info, "gateway", "shutdown", "packets_rejected", static_cast<std::int64_t>(packetsRejected));
    satgcs::util::Logger::logKv(Level::Info, "gateway", "shutdown", "retry_queue_remaining", static_cast<std::int64_t>(retryQueue.size()));
    close(fd);
    return 0;
}
