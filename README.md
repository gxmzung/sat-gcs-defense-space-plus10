# SAT-GCS Defense/Space Style Portfolio (+10 Operational Upgrade)

Mini Satellite Ground Control System built as a **defense/space software engineering portfolio**.

This repository is not certified flight/mission software. It is a study project that imitates defense/aerospace development practices: requirements, traceability, interface control, verification planning, structured logging, configuration separation, validation, and operational runbooks.


## +10 Operational Upgrade

This package includes additional production-style work:

- C++ POSIX HTTP client instead of shelling out to `curl`
- Gateway timeout / retry / bounded queue
- Graceful shutdown with SIGINT/SIGTERM
- Structured JSON logs with millisecond timestamps
- Fault-injection UDP test tool
- Expanded RTM/SRS/VTP traceability
- IDE and run command guide: `docs/IDE_AND_RUNBOOK.md`
- Career level guide: `docs/CAREER_LEVEL_GUIDE.md`

## Architecture

```text
[C++ Satellite Device Simulator]
    UDP binary telemetry + CRC32
        ↓
[C++ Telemetry Gateway]
    size/magic/version/CRC validation
    retry queue
    JSON forwarding
        ↓ HTTP + API Key
[Java Spring Boot Mission Server]
    validation / persistence / alerting / health check
        ↓
[React Dashboard]      [C# Operator Console]
        ↑
[Python Replay / Load / Schema Tools]
```

## Why this project exists

The goal is to connect hardware-production experience with ground-system software:

- PCB/Gerber/BOM/datasheet/material flow experience
- device data generation
- binary protocol handling
- telemetry ingestion
- mission server and operator view
- verification and traceability documents

## Key Documents

| Document | Purpose |
|---|---|
| `docs/SRS.md` | Software requirements |
| `docs/ICD.md` | UDP/HTTP interface control |
| `docs/SDD.md` | Software design description |
| `docs/VTP.md` | Verification test plan |
| `docs/VTR.md` | Verification report template |
| `docs/RTM.md` | Requirements traceability matrix |
| `docs/CODING_STANDARD.md` | Lightweight coding standard profile |
| `docs/RISK_REGISTER.md` | Risk and mitigation table |
| `docs/OPERATIONS_RUNBOOK.md` | Build/run/fault handling guide |
| `docs/SECURITY_NOTES.md` | Security boundary and production extensions |
| `docs/PORTFOLIO_GUIDE.md` | Interview explanation guide |

## Quick Start

```bash
cp .env.example .env
docker compose up --build
```

Dashboard:

```text
http://localhost:5173
```

Mission server health:

```bash
curl http://localhost:8080/actuator/health
```

## Build and Run C++ Layer

```bash
make cpp-build
make cpp-test

./build/cpp/telemetry_gateway \
  --listen 0.0.0.0 \
  --port 46000 \
  --server http://localhost:8080 \
  --api-key dev-api-key

./build/cpp/satellite_device_sim --host 127.0.0.1 --port 46000
```

## Run Python Replay Tool

```bash
cd python-tools
python -m venv .venv
. .venv/bin/activate
pip install -r requirements.txt
python telemetry_replay.py --api http://localhost:8080 --api-key dev-api-key --file sample_telemetry.jsonl
```

## What makes this more practical than a toy app?

- Binary UDP telemetry packet instead of plain JSON device data
- CRC32 packet integrity check
- Explicit decode error status in C++
- Bounded retry queue in gateway
- Mission server API key boundary
- DTO validation and DB migration
- Health check endpoints
- Replay/load/schema tools
- Traceability matrix from requirements to code/tests
- Interface Control Document and Verification Test Plan

## Honest Limits

This project does **not** claim:

- real CCSDS/ECSS full implementation
- real satellite flight software
- certified defense/aerospace compliance
- hardware-in-the-loop validation
- formal timing analysis
- production-grade mission security

Interview wording:

> This is not certified mission software. I built it to study how defense/space-style systems are structured: requirements, ICD, C++ protocol layer, mission server, operator tools, and verification traceability.
