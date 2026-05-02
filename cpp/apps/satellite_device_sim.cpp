#include "protocol/TelemetryPacket.hpp"
#include "math/AttitudeMath.hpp"
#include "util/Logger.hpp"
#include <arpa/inet.h>
#include <chrono>
#include <cstring>
#include <random>
#include <string>
#include <thread>
#include <unistd.h>
using namespace satgcs;

int main(int argc, char** argv) {
    std::string host="127.0.0.1"; int port=46000;
    for(int i=1;i<argc;i++){ std::string a=argv[i]; if(a=="--host"&&i+1<argc) host=argv[++i]; else if(a=="--port"&&i+1<argc) port=std::stoi(argv[++i]); }
    int fd=socket(AF_INET, SOCK_DGRAM, 0); if(fd<0){ perror("socket"); return 1; }
    sockaddr_in addr{}; addr.sin_family=AF_INET; addr.sin_port=htons(port); inet_pton(AF_INET, host.c_str(), &addr.sin_addr);
    std::mt19937 rng(std::random_device{}()); std::normal_distribution<double> noise(0.0, 1.0);
    uint32_t seq=1; auto start = std::chrono::steady_clock::now();
    util::Logger::log(util::Level::Info,"device-sim","started");
    while(true){
        auto now=std::chrono::steady_clock::now(); double t=std::chrono::duration<double>(now-start).count(); auto e=math::syntheticAttitude(t);
        protocol::TelemetryPacket p; p.sequence=seq++; p.unixMs=protocol::nowUnixMs();
        p.batteryMv = static_cast<uint16_t>(7400 - (seq % 500));
        p.temperatureCentiC = static_cast<int16_t>((32.0 + 5.0*std::sin(t/20.0) + noise(rng))*100);
        p.rollMdeg=e.roll*1000; p.pitchMdeg=e.pitch*1000; p.yawMdeg=e.yaw*1000;
        p.latE7 = static_cast<int32_t>((36.3500 + 0.001*std::sin(t/25.0))*10000000);
        p.lonE7 = static_cast<int32_t>((127.3845 + 0.001*std::cos(t/25.0))*10000000);
        p.status = p.temperatureCentiC > 3700 ? protocol::Status::Warning : protocol::Status::Normal;
        auto buf=p.encode();
        sendto(fd, buf.data(), buf.size(), 0, (sockaddr*)&addr, sizeof(addr));
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}
