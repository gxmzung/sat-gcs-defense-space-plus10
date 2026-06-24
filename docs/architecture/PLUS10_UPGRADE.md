# +10% 실무화 업그레이드 노트

이 버전은 기존 준실무 포트폴리오에서 운영 안정성·장애 대응·테스트 깊이를 보강한 버전이다.

## 추가/개선 항목

| 영역 | 변경 내용 | 실무 의미 |
|---|---|---|
| C++ Gateway | `curl` 쉘 호출 제거, POSIX socket 기반 `HttpClient` 추가 | 외부 프로세스 의존 제거, timeout 제어 가능 |
| C++ Gateway | connect/read timeout 추가 | 서버 장애 시 gateway가 무한 대기하지 않음 |
| C++ Gateway | retry/backoff 추가 | 일시적 장애 복구 가능 |
| C++ Gateway | bounded retry queue | 메모리 무한 증가 방지 |
| C++ Gateway | SIGINT/SIGTERM graceful shutdown | 운영 중 안전 종료 |
| Logging | millisecond timestamp JSON logging | 로그 수집/검색 친화 |
| Protocol Test | invalid magic/version/status with valid CRC 테스트 추가 | CRC 뒤의 의미 검증까지 확인 |
| Fault Injection | `python-tools/fault_inject_udp.py` 추가 | 깨진 UDP 패킷에도 gateway가 죽지 않는지 검증 |

## 실무화 퍼센트 재평가

냉정한 포트폴리오 기준:

- 이전: 55~65%
- 현재: 68~75%

아직 실제 방산/우주 인증품은 아니다. 하지만 면접 포트폴리오로는 "운영 안정성까지 생각했다"고 말할 수 있는 수준으로 올라간다.

## 아직 부족한 것

- TLS/mTLS 실적용
- Prometheus metrics endpoint
- 완전한 MISRA C++ 준수 검증
- 코드 리뷰 기록
- 요구사항별 자동 시험 성적서 생성
- 실제 장비/시리얼/CAN/Ethernet 연동

## Visual Mission Upgrade

This version adds an operator-facing mission visualization layer:

- Launch sequence visualization: countdown, liftoff, boost ascent, stage separation, orbit insertion, telemetry link.
- 2D orbit/ground-track view: satellite position, recent path, Daejeon ground station marker, visibility status.
- Mission console log: operator-style event stream for launch and telemetry events.

These are frontend visualizations intended for portfolio demonstration and operator situational awareness. They do not represent a certified flight dynamics or launch simulation model.
