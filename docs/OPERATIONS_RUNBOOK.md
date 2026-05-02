# Operations Runbook

## Start Full Stack

```bash
cp .env.example .env
docker compose up --build
```

## Build C++

```bash
make cpp-build
make cpp-test
```

## Run Gateway

```bash
./build/cpp/telemetry_gateway \
  --listen 0.0.0.0 \
  --port 46000 \
  --server http://localhost:8080 \
  --api-key dev-api-key
```

## Run Device Simulator

```bash
./build/cpp/satellite_device_sim --host 127.0.0.1 --port 46000
```

## Replay Telemetry

```bash
cd python-tools
python -m venv .venv
. .venv/bin/activate
pip install -r requirements.txt
python telemetry_replay.py --api http://localhost:8080 --api-key dev-api-key --file sample_telemetry.jsonl
```

## Health Check

```bash
curl http://localhost:8080/actuator/health
```

## Common Faults

| Symptom | Probable Cause | Action |
|---|---|---|
| Gateway logs forward failed | Mission server down or API key mismatch | Check server health and `.env` |
| No dashboard data | No telemetry ingested | Run simulator or replay tool |
| CRC validation failed | Corrupt packet or protocol mismatch | Check simulator/gateway versions |
| HTTP 401 | Missing/invalid API key | Set `X-API-Key` header |

## Gateway Failure Modes Added in +10 Upgrade

### Mission server unavailable

Symptom:

```text
{"event":"forward_failed", ...}
{"event":"queued_for_retry", ...}
```

Action:

1. Check mission server health: `curl http://localhost:8080/actuator/health`.
2. Check API key in `.env` and gateway command.
3. Restart mission server if health check fails.
4. Keep gateway running; it drains retry queue after server recovery.

### Malformed packet storm

Symptom:

```text
{"event":"packet_rejected_invalid_size", ...}
{"event":"packet_rejected_protocol", ...}
```

Action:

1. Check upstream device/simulator version.
2. Compare packet format with `docs/ICD.md`.
3. Run `python-tools/fault_inject_udp.py` to reproduce rejection behavior.
