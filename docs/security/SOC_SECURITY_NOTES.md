# SOC Security Notes

## Purpose

This document summarizes the security perspective of the SAT-GCS Defense Space Plus10 prototype.

The goal is to connect satellite telemetry, mission server design, and operator dashboard behavior with security monitoring concepts.

## Security Monitoring Targets

Potential SOC monitoring targets include:

- abnormal telemetry packet frequency
- malformed packet payloads
- unexpected mission-state transition
- replayed telemetry messages
- operator dashboard access anomalies
- mission server error spikes
- network latency or packet-loss anomalies

## Example Detection Ideas

| Area | Detection Idea |
|---|---|
| Telemetry | Detect malformed or missing fields |
| Mission State | Detect impossible state transitions |
| Network | Detect repeated replay traffic |
| Dashboard | Detect abnormal operator actions |
| Backend | Detect API error spikes |
| Logs | Detect unusual command sequence patterns |

## Threat Model

This prototype assumes the following possible threats:

- telemetry spoofing
- packet replay
- malformed packet injection
- dashboard misuse
- mission server overload
- weak validation logic
- insecure test data handling

## Defensive Direction

Future improvements may include:

- telemetry packet signature validation
- replay protection
- anomaly scoring
- role-based dashboard access
- structured audit logs
- SIEM-friendly log format
- incident timeline generation

## Boundary

This document is for educational and portfolio purposes.

It does not describe classified systems, real satellite access, or operational defense procedures.
