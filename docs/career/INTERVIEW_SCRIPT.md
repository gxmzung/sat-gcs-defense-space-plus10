# Interview Script

## 30 seconds
회로설계 경험을 하며 실제 하드웨어가 생산되고 디버깅되는 흐름을 봤습니다. 이 프로젝트는 그 경험을 소프트웨어 관제 흐름으로 확장한 것입니다. C++ 시뮬레이터가 바이너리 텔레메트리를 만들고, C++ 게이트웨이가 UDP 수신·CRC 검증·디코딩을 한 뒤 Java 서버로 전달합니다. 서버는 DB 저장과 알람 판정을 하고, React와 C# 콘솔이 운영자 화면 역할을 합니다.

## Why this matters
단순 CRUD가 아니라 장비 통신 계층, 운영 서버, 관제 UI, 시험 자동화가 분리되어 있다는 점이 중요합니다.

## What I would improve next
CCSDS 스타일 프레임, Prometheus metrics, JWT/RBAC, WebSocket push, packet loss simulation, NTP clock drift handling.
