# Verification Test Plan (VTP)

## 1. Verification Strategy

The project uses layered verification:

1. C++ protocol unit tests
2. Java API validation tests/manual curl checks
3. Python schema/replay tests
4. Docker smoke test
5. Fault injection tests
6. Manual UI inspection

## 2. Test Cases

| Test ID | Requirement | Procedure | Expected Result |
|---|---|---|---|
| TEST-PROTO-001 | REQ-GCS-002 | Encode then decode a packet | Decoded fields match source |
| TEST-PROTO-002 | NFR-GCS-002 | Decode malformed buffer | No crash, explicit failure |
| TEST-GW-001 | REQ-GCS-003 | Send short UDP packet | Gateway logs invalid size and drops |
| TEST-GW-002 | REQ-GCS-004 | Corrupt CRC field | Gateway rejects packet |
| TEST-GW-003 | REQ-GCS-005 | Alter magic/version | Gateway rejects packet |
| TEST-GW-004 | REQ-GCS-007 | Stop mission server and send packets | Gateway queues failed forwards |
| TEST-INT-001 | REQ-GCS-006 | Run simulator, gateway, server | Server receives telemetry |
| TEST-SERVER-001 | REQ-GCS-008 | POST invalid JSON | HTTP 400 |
| TEST-SERVER-002 | REQ-GCS-009 | POST valid JSON | Record persisted |
| TEST-SERVER-003 | REQ-GCS-010 | POST high temperature | Alert created |
| TEST-SERVER-004 | REQ-GCS-011 | POST low battery | Alert created |
| TEST-API-001 | REQ-GCS-012 | GET latest telemetry | Latest object returned |
| TEST-UI-001 | REQ-GCS-013 | Open dashboard | Latest status visible |
| TEST-OPS-001 | REQ-GCS-014 | Run C# console | Health/latest data printed |
| TEST-AUTO-001 | REQ-GCS-015 | Run replay tool | Sample telemetry ingested |

## 3. Fault Injection

- Corrupt one byte after CRC generation.
- Send only 10 bytes to UDP port.
- Stop mission server while gateway is running.
- Use invalid API key.
- Submit impossible latitude/longitude.

## 4. Exit Criteria

- C++ protocol tests pass.
- Docker stack starts.
- Health endpoint returns UP.
- Replay tool ingests sample telemetry.
- Dashboard loads without runtime error.
- At least one alert condition verified.

## +10 Upgrade Test Cases

| Test ID | Purpose | Procedure | Expected Result |
|---|---|---|---|
| VTP-010 | Forward timeout | Start gateway while mission server is down | Gateway logs `forward_failed` and continues running |
| VTP-011 | Retry queue bound | Run gateway with `--max-queue 3` and server down, then run simulator | Gateway logs queue full/drop after queue is full |
| VTP-012 | Malformed UDP fault injection | Run `python-tools/fault_inject_udp.py` | Gateway rejects packets and stays alive |
| VTP-013 | Graceful shutdown | Press Ctrl+C while gateway is running | Gateway logs shutdown counters |
| VTP-014 | Protocol edge cases | Run `./build/cpp/protocol_tests` | All protocol tests pass |
