# Requirements Traceability Matrix (RTM)

| Requirement | Design Element | Code | Test/Verification |
|---|---|---|---|
| REQ-GCS-001 | Device Simulator | `cpp/apps/satellite_device_sim.cpp` | `scripts/smoke-test.sh` |
| REQ-GCS-002 | Binary Telemetry Protocol | `cpp/protocol/TelemetryPacket.hpp/.cpp` | `cpp/tests/protocol_tests.cpp` |
| REQ-GCS-003 | Packet Size Validation | `TelemetryPacket::decode` | `protocol_tests.cpp::rejects_invalid_size` |
| REQ-GCS-004 | CRC Validation | `Crc32.cpp`, `TelemetryPacket::decode` | `protocol_tests.cpp::rejects_corrupted_packet` |
| REQ-GCS-005 | Magic/Version Validation | `TelemetryPacket::decode` | `protocol_tests.cpp::rejects_invalid_magic` |
| REQ-GCS-006 | Forwarding to Mission Server | `cpp/apps/telemetry_gateway.cpp` | `scripts/smoke-test.sh` |
| REQ-GCS-007 | Retry Queue | `telemetry_gateway.cpp` | Manual fault injection: stop server, run gateway |
| REQ-GCS-008 | Payload Validation | `TelemetryRequest.java` | Java validation on POST `/api/telemetry` |
| REQ-GCS-009 | Persistence | `TelemetryEntity`, `TelemetryRepository` | API smoke test and DB inspection |
| REQ-GCS-010 | Temperature Alert | `TelemetryService.java` | Post payload with high temperature |
| REQ-GCS-011 | Battery Alert | `TelemetryService.java` | Post payload with low battery |
| REQ-GCS-012 | Latest Telemetry API | `TelemetryController.java` | `curl /api/telemetry/latest` |
| REQ-GCS-013 | Dashboard | `frontend/src/App.tsx` | Browser check |
| REQ-GCS-014 | Operator Console | `dotnet-operator-console/Program.cs` | `dotnet run` |
| REQ-GCS-015 | Replay Tool | `python-tools/telemetry_replay.py` | Run replay against server |

## +10 Upgrade Traceability

| Requirement | Code | Test/Verification |
|---|---|---|
| REQ-GCS-010 | `cpp/util/HttpClient.cpp`, `cpp/apps/telemetry_gateway.cpp` | Gateway command with low timeout against stopped server |
| REQ-GCS-011 | `postWithRetry()` in `telemetry_gateway.cpp` | Stop mission server and observe retry logs |
| REQ-GCS-012 | `retryQueue` max queue check in `telemetry_gateway.cpp` | Code inspection, high-rate failure test |
| REQ-GCS-013 | `TelemetryPacket::decodeDetailed`, gateway reject branches | `python-tools/fault_inject_udp.py`, `cpp/tests/protocol_tests.cpp` |
| REQ-GCS-014 | `handleSignal`, `g_running` in `telemetry_gateway.cpp` | Ctrl+C during gateway execution |
| REQ-GCS-015 | `cpp/util/Logger.cpp` | Log inspection |
