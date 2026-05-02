#pragma once
#include <chrono>
#include <cstdint>
#include <string>

namespace satgcs::util {

struct HttpResult final {
    bool ok{false};
    int statusCode{0};
    std::string error{};
};

struct HttpClientConfig final {
    std::chrono::milliseconds connectTimeout{1000};
    std::chrono::milliseconds readTimeout{2000};
    std::uint16_t defaultPort{80U};
};

// Minimal POSIX HTTP/1.1 client used by the gateway to avoid shelling out to curl.
// Traceability: REQ-GCS-010, REQ-GCS-011, REQ-GCS-012
class HttpClient final {
public:
    explicit HttpClient(HttpClientConfig config = {});
    [[nodiscard]] HttpResult postJson(const std::string& baseUrl,
                                      const std::string& path,
                                      const std::string& apiKey,
                                      const std::string& json) const noexcept;
private:
    HttpClientConfig config_{};
};

} // namespace satgcs::util
