# IDE와 실행 명령어

## 추천 IDE

| 영역 | 추천 IDE | 이유 |
|---|---|---|
| C/C++ | CLion 또는 VS Code + CMake Tools | CMake, 디버깅, 헤더 추적이 편함 |
| Java Spring Boot | IntelliJ IDEA Community/Ultimate | Spring 프로젝트 구조 파악이 가장 편함 |
| React | VS Code | 프론트엔드 작업에 충분함 |
| Python/Shell | VS Code | 스크립트 실행/수정에 편함 |
| C#/.NET | Visual Studio 또는 Rider | Windows 환경이면 Visual Studio가 편함 |

현실적으로 하나만 고르면 `VS Code`로 전체를 열고, Java만 IntelliJ로 열면 된다.

## 필수 도구

- Docker Desktop
- JDK 21
- CMake 3.20+
- GCC/G++ 11+ 또는 Clang
- Python 3.10+
- Node.js 20+
- .NET SDK 8

## 전체 실행

```bash
cp .env.example .env
docker compose up --build
```

대시보드:

```text
http://localhost:5173
```

## C++ 빌드/테스트

```bash
make cpp-build
./build/cpp/protocol_tests
```

## Gateway 실행

```bash
./build/cpp/telemetry_gateway \
  --listen 0.0.0.0 \
  --port 46000 \
  --server http://localhost:8080 \
  --api-key dev-api-key \
  --connect-timeout-ms 1000 \
  --read-timeout-ms 2000 \
  --retry-attempts 3 \
  --max-queue 1000
```

## 위성 장비 시뮬레이터 실행

```bash
./build/cpp/satellite_device_sim --host 127.0.0.1 --port 46000
```

## 장애 패킷 주입 테스트

다른 터미널에서 gateway를 켠 뒤 실행:

```bash
python3 python-tools/fault_inject_udp.py --host 127.0.0.1 --port 46000 --count 50
```

기대 결과:

- gateway가 종료되지 않아야 한다.
- 로그에 `packet_rejected_invalid_size` 또는 `packet_rejected_protocol`이 찍혀야 한다.

## Java 테스트

```bash
cd java-mission-server
./mvnw test
```

## Python 도구 실행

```bash
cd python-tools
python -m venv .venv
source .venv/bin/activate
pip install -r requirements.txt
python telemetry_replay.py --api http://localhost:8080 --api-key dev-api-key
```

## C# 운영자 콘솔

```bash
cd dotnet-operator-console
dotnet run -- --api http://localhost:8080
```
