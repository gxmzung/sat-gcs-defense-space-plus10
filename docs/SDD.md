# Software Design Description (SDD)

## 1. Architecture

```text
[Satellite Device Simulator - C++]
    UDP binary packet + CRC
        ↓
[Telemetry Gateway - C++]
    decode / validate / convert / retry
        ↓ HTTP JSON
[Mission Server - Java Spring Boot]
    validate / persist / alert / expose API
        ↓
[React Dashboard]  [C# Operator Console]
        ↑
[Python Replay/Load/Schema Tools]
```

## 2. Design Rationale

### C++
Used for device-near and protocol code because this layer simulates real-time-ish telemetry generation, binary protocol handling, CRC validation, and gateway operations.

### Java
Used for mission server because enterprise operational systems often need structured APIs, persistence, validation, health checks, and maintainable service-layer design.

### Python/Shell
Used for automation, replay, fault injection, and smoke testing.

### C#/.NET
Used as an operator console/internal tool example.

## 3. C++ Gateway Design

Responsibilities:

- bind UDP socket
- receive fixed-size packets
- validate packet size
- decode packet with explicit result
- verify magic/version/CRC
- serialize valid packet to JSON
- forward to mission server
- retain failed forwards in bounded retry queue
- log operational events

Failure handling:

| Failure | Handling |
|---|---|
| Invalid size | Drop and log warning |
| CRC mismatch | Drop and log warning |
| Mission server down | Queue up to configured limit |
| Queue full | Drop oldest/newest depending configuration; current implementation drops new when full |

## 4. Mission Server Design

Layers:

- Controller: API boundary
- DTO: input validation
- Service: business rules and alert generation
- Repository: persistence access
- Entity: database model

Alert conditions:

- `temperature >= SATGCS_TEMP_WARNING_C`
- `batteryVoltage <= SATGCS_BATTERY_LOW_V`
- `status == CRITICAL`

## 5. Security Design

Implemented:

- API key filter for ingest/query APIs
- Environment-based secret configuration
- Input validation

Not implemented but documented for extension:

- TLS termination
- mTLS between gateway and server
- Role-based operator authentication
- Audit log signing

## 6. Maintainability

- Requirements IDs are referenced in tests/docs.
- Interface is documented in ICD.
- CI and Makefile provide repeatable commands.
- `.env.example` prevents hard-coded local secrets.
