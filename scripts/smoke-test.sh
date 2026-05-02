#!/usr/bin/env bash
set -euo pipefail
API=${API:-http://localhost:8080}
KEY=${SATGCS_API_KEY:-dev-api-key}
curl -fsS "$API/actuator/health" | grep UP
curl -fsS -X POST "$API/api/telemetry" -H "Content-Type: application/json" -H "X-API-Key: $KEY" -d '{"satelliteId":"KOMPSAT-SIM-01","sequence":777,"timestampMs":1710000000000,"batteryVoltage":7.4,"temperature":32.0,"roll":0,"pitch":0,"yaw":90,"lat":36.35,"lon":127.38,"status":"NORMAL"}'
curl -fsS "$API/api/telemetry/latest" | grep KOMPSAT-SIM-01
