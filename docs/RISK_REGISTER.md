# Risk Register

| ID | Risk | Impact | Likelihood | Mitigation |
|---|---|---:|---:|---|
| RISK-001 | UDP packet loss | Medium | Medium | Sequence numbers, monitoring, replay/fault tests |
| RISK-002 | Corrupted telemetry accepted | High | Low | CRC32 and protocol validation |
| RISK-003 | Mission server unavailable | High | Medium | Gateway retry queue, health checks |
| RISK-004 | Invalid payload stored | Medium | Low | Java validation and DB constraints |
| RISK-005 | Hard-coded secrets | Medium | Medium | `.env.example`, environment config |
| RISK-006 | Operator misreads stale data | Medium | Medium | Timestamp display, latest endpoint |
| RISK-007 | Scope overclaim in interview | High | Medium | Clearly state portfolio/non-certified limits |
