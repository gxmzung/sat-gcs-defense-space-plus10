# Fault Injection Scenarios

## Scenario 1: Invalid CRC

Expected behavior:

- Packet is rejected
- Error is logged
- Dashboard should not treat the packet as valid telemetry

## Scenario 2: Missing Telemetry

Expected behavior:

- System detects stale data
- Operator receives warning state

## Scenario 3: Abnormal Sensor Value

Expected behavior:

- Value is classified as abnormal
- Event is recorded for operator review

## Scenario 4: Gateway Disconnection

Expected behavior:

- Server keeps running
- Dashboard shows degraded state
