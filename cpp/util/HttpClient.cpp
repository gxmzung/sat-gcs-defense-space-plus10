#include "util/HttpClient.hpp"
#include <arpa/inet.h>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <netdb.h>
#include <sstream>
#include <string_view>
#include <sys/select.h>
#include <sys/socket.h>
#include <unistd.h>

namespace satgcs::util {
namespace {
struct ParsedUrl final {
    std::string host{};
    std::uint16_t port{80U};
};

bool parseHttpUrl(const std::string& url, std::uint16_t defaultPort, ParsedUrl& out) {
    constexpr std::string_view prefix{"http://"};
    if (url.rfind(prefix.data(), 0U) != 0U) {
        return false;
    }
    std::string authority = url.substr(prefix.size());
    const auto slashPos = authority.find('/');
    if (slashPos != std::string::npos) {
        authority = authority.substr(0U, slashPos);
    }
    const auto colonPos = authority.find(':');
    out.port = defaultPort;
    if (colonPos != std::string::npos) {
        out.host = authority.substr(0U, colonPos);
        const auto portText = authority.substr(colonPos + 1U);
        try {
            const auto parsed = std::stoi(portText);
            if (parsed <= 0 || parsed > 65535) {
                return false;
            }
            out.port = static_cast<std::uint16_t>(parsed);
        } catch (...) {
            return false;
        }
    } else {
        out.host = authority;
    }
    return !out.host.empty();
}

bool setNonBlocking(int fd) noexcept {
    const int flags = fcntl(fd, F_GETFL, 0);
    return flags >= 0 && fcntl(fd, F_SETFL, flags | O_NONBLOCK) == 0;
}

bool waitForFd(int fd, bool write, std::chrono::milliseconds timeout) noexcept {
    fd_set set;
    FD_ZERO(&set);
    FD_SET(fd, &set);
    timeval tv{};
    tv.tv_sec = static_cast<long>(timeout.count() / 1000);
    tv.tv_usec = static_cast<long>((timeout.count() % 1000) * 1000);
    const int rc = select(fd + 1, write ? nullptr : &set, write ? &set : nullptr, nullptr, &tv);
    return rc > 0 && FD_ISSET(fd, &set);
}

bool connectWithTimeout(int fd, const sockaddr* addr, socklen_t len, std::chrono::milliseconds timeout) noexcept {
    if (!setNonBlocking(fd)) {
        return false;
    }
    const int rc = connect(fd, addr, len);
    if (rc == 0) {
        return true;
    }
    if (errno != EINPROGRESS) {
        return false;
    }
    if (!waitForFd(fd, true, timeout)) {
        return false;
    }
    int err = 0;
    socklen_t errLen = sizeof(err);
    if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &errLen) != 0) {
        return false;
    }
    return err == 0;
}

bool sendAll(int fd, const std::string& data, std::chrono::milliseconds timeout) noexcept {
    std::size_t sent = 0U;
    while (sent < data.size()) {
        if (!waitForFd(fd, true, timeout)) {
            return false;
        }
        const auto n = send(fd, data.data() + sent, data.size() - sent, MSG_NOSIGNAL);
        if (n <= 0) {
            return false;
        }
        sent += static_cast<std::size_t>(n);
    }
    return true;
}

int parseStatusCode(const std::string& response) noexcept {
    const auto firstSpace = response.find(' ');
    if (firstSpace == std::string::npos || firstSpace + 4U > response.size()) {
        return 0;
    }
    try {
        return std::stoi(response.substr(firstSpace + 1U, 3U));
    } catch (...) {
        return 0;
    }
}
} // namespace

HttpClient::HttpClient(HttpClientConfig config) : config_(config) {}

HttpResult HttpClient::postJson(const std::string& baseUrl,
                                const std::string& path,
                                const std::string& apiKey,
                                const std::string& json) const noexcept {
    ParsedUrl target{};
    if (!parseHttpUrl(baseUrl, config_.defaultPort, target)) {
        return {.ok = false, .statusCode = 0, .error = "unsupported_or_invalid_url"};
    }

    addrinfo hints{};
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    addrinfo* result = nullptr;
    const auto portText = std::to_string(target.port);
    if (getaddrinfo(target.host.c_str(), portText.c_str(), &hints, &result) != 0 || result == nullptr) {
        return {.ok = false, .statusCode = 0, .error = "dns_resolution_failed"};
    }

    int fd = -1;
    bool connected = false;
    for (auto* rp = result; rp != nullptr; rp = rp->ai_next) {
        fd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
        if (fd < 0) {
            continue;
        }
        if (connectWithTimeout(fd, rp->ai_addr, static_cast<socklen_t>(rp->ai_addrlen), config_.connectTimeout)) {
            connected = true;
            break;
        }
        close(fd);
        fd = -1;
    }
    freeaddrinfo(result);
    if (!connected || fd < 0) {
        return {.ok = false, .statusCode = 0, .error = "connect_timeout_or_refused"};
    }

    std::ostringstream request;
    request << "POST " << path << " HTTP/1.1\r\n"
            << "Host: " << target.host << ':' << target.port << "\r\n"
            << "Content-Type: application/json\r\n"
            << "X-API-Key: " << apiKey << "\r\n"
            << "Connection: close\r\n"
            << "Content-Length: " << json.size() << "\r\n\r\n"
            << json;

    if (!sendAll(fd, request.str(), config_.readTimeout)) {
        close(fd);
        return {.ok = false, .statusCode = 0, .error = "send_failed_or_timeout"};
    }

    std::string response;
    char buffer[512]{};
    while (waitForFd(fd, false, config_.readTimeout)) {
        const auto n = recv(fd, buffer, sizeof(buffer), 0);
        if (n <= 0) {
            break;
        }
        response.append(buffer, static_cast<std::size_t>(n));
        if (response.size() > 4096U) {
            break;
        }
    }
    close(fd);

    const int statusCode = parseStatusCode(response);
    const bool ok = statusCode >= 200 && statusCode < 300;
    return {.ok = ok, .statusCode = statusCode, .error = ok ? std::string{} : "http_non_2xx_or_no_response"};
}

} // namespace satgcs::util
