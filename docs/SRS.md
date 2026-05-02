# Software Requirements Specification (SRS)

Project: SAT-GCS Defense/Space Style Portfolio  
Version: 1.0  
Scope: Mini satellite ground control system with simulated satellite telemetry, C++ gateway, Java mission server, React dashboard, C# operator console, and Python/Shell verification tools.

> This project is not certified defense/aerospace software. It is a portfolio project that imitates defense/space software engineering practices: requirements, traceability, interface control, verification, logging, configuration, and fault handling.

## 1. Operational Concept

A simulated satellite device periodically sends binary telemetry packets over UDP. A ground telemetry gateway receives packets, validates protocol fields and CRC, converts valid packets to JSON, and forwards them to the mission server. The mission server stores telemetry, evaluates alert conditions, and exposes APIs for dashboards and operator tools.

## 2. Functional Requirements

| ID | Requirement | Verification |
|---|---|---|
| REQ-GCS-001 | The device simulator shall emit telemetry packets at a configurable interval. | TEST-SIM-001 |
| REQ-GCS-002 | The telemetry packet shall use a fixed binary format with magic, version, sequence, payload, and CRC32. | TEST-PROTO-001 |
| REQ-GCS-003 | The gateway shall reject packets with invalid size. | TEST-GW-001 |
| REQ-GCS-004 | The gateway shall reject packets with invalid CRC. | TEST-GW-002 |
| REQ-GCS-005 | The gateway shall reject packets with invalid magic or unsupported protocol version. | TEST-GW-003 |
| REQ-GCS-006 | The gateway shall forward valid telemetry to the mission server using HTTP JSON. | TEST-INT-001 |
| REQ-GCS-007 | If forwarding fails, the gateway shall place telemetry into a bounded retry queue. | TEST-GW-004 |
| REQ-GCS-008 | The mission server shall validate incoming telemetry payloads before persistence. | TEST-SERVER-001 |
| REQ-GCS-009 | The mission server shall persist valid telemetry in PostgreSQL. | TEST-SERVER-002 |
| REQ-GCS-010 | The mission server shall create an alert when temperature exceeds the configured threshold. | TEST-SERVER-003 |
| REQ-GCS-011 | The mission server shall create an alert when battery voltage falls below the configured threshold. | TEST-SERVER-004 |
| REQ-GCS-012 | The mission server shall expose the latest telemetry through an API. | TEST-API-001 |
| REQ-GCS-013 | The dashboard shall show latest telemetry and alert list. | TEST-UI-001 |
| REQ-GCS-014 | The operator console shall query mission server health and latest telemetry. | TEST-OPS-001 |
| REQ-GCS-015 | Python tools shall replay sample telemetry for repeatable integration tests. | TEST-AUTO-001 |

## 3. Non-Functional Requirements

| ID | Requirement | Verification |
|---|---|---|
| NFR-GCS-001 | C++ protocol code shall use fixed-width integer types for wire compatibility. | REVIEW-CODE-001 |
| NFR-GCS-002 | Protocol decode shall return explicit result status, not crash on malformed input. | TEST-PROTO-002 |
| NFR-GCS-003 | Gateway shall log significant events in structured one-line records. | REVIEW-LOG-001 |
| NFR-GCS-004 | Secrets and endpoints shall be configurable by environment variables or CLI flags. | REVIEW-CONFIG-001 |
| NFR-GCS-005 | The system shall provide health endpoints for operational checking. | TEST-OPS-002 |
| NFR-GCS-006 | CI shall build/test C++, Java, Python schema checks, and frontend type checks where tools are available. | TEST-CI-001 |

## 4. Assumptions and Limits

- Real satellite standards such as CCSDS, ECSS packet utilization, time correlation, and cryptographic mission security are not fully implemented.
- UDP is used to demonstrate lossy device communication. Production systems often use additional transport, redundancy, and secured links.
- Thresholds are simplified for portfolio demonstration.
- This is not safety-critical certified software.

## 8. Operational Resilience Requirements (+10 Upgrade)

| ID | Requirement | Verification |
|---|---|---|
| REQ-GCS-010 | Gateway shall apply connection and read timeouts when forwarding telemetry to mission server. | Code inspection, integration test |
| REQ-GCS-011 | Gateway shall retry failed telemetry forwards with bounded attempts. | Fault injection test |
| REQ-GCS-012 | Gateway shall not grow retry memory without bound. | Code inspection |
| REQ-GCS-013 | Gateway shall reject malformed UDP packets without process termination. | Fault injection test |
| REQ-GCS-014 | Gateway shall support graceful shutdown on SIGINT/SIGTERM. | Manual test |
| REQ-GCS-015 | Gateway shall produce structured JSON logs for operational events. | Log inspection |
