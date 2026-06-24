# Interface Control Document (ICD)

## 1. UDP Telemetry Interface

Producer: C++ Satellite Device Simulator  
Consumer: C++ Telemetry Gateway  
Transport: UDP  
Default Port: 46000  
Byte Order: Little endian  
Packet Size: 64 bytes fixed

## 2. Binary Packet Layout

| Offset | Size | Type | Field | Description |
|---:|---:|---|---|---|
| 0 | 2 | uint16 | magic | `0x5347` = 'SG' |
| 2 | 1 | uint8 | version | Current version = 1 |
| 3 | 1 | uint8 | flags | Reserved |
| 4 | 4 | uint32 | sequence | Monotonic sequence |
| 8 | 16 | char[16] | satellite_id | Null-padded ASCII |
| 24 | 8 | int64 | timestamp_ms | Unix epoch milliseconds |
| 32 | 2 | uint16 | battery_mv | Millivolts |
| 34 | 2 | int16 | temperature_centi_c | Celsius × 100 |
| 36 | 4 | int32 | roll_mdeg | Degrees × 1000 |
| 40 | 4 | int32 | pitch_mdeg | Degrees × 1000 |
| 44 | 4 | int32 | yaw_mdeg | Degrees × 1000 |
| 48 | 4 | int32 | lat_e7 | Latitude × 10^7 |
| 52 | 4 | int32 | lon_e7 | Longitude × 10^7 |
| 56 | 1 | uint8 | status | 0=NORMAL, 1=WARNING, 2=CRITICAL |
| 57 | 3 | uint8[3] | reserved | Reserved |
| 60 | 4 | uint32 | crc32 | CRC32 over bytes 0..59 |

## 3. HTTP Telemetry Interface

Producer: C++ Telemetry Gateway / Python Replay Tool  
Consumer: Java Mission Server  
Endpoint: `POST /api/telemetry`  
Header: `X-API-Key: <key>`  
Content-Type: `application/json`

### JSON Payload

```json
{
  "satelliteId": "KOMPSAT-SIM-01",
  "sequence": 42,
  "timestampMs": 1770000000000,
  "batteryVoltage": 7.41,
  "temperature": 32.12,
  "roll": 1.200,
  "pitch": -0.400,
  "yaw": 80.500,
  "lat": 36.3500000,
  "lon": 127.3800000,
  "status": "NORMAL"
}
```

## 4. REST Query APIs

| Method | Path | Purpose |
|---|---|---|
| GET | `/actuator/health` | Mission server health |
| POST | `/api/telemetry` | Ingest telemetry |
| GET | `/api/telemetry/latest` | Latest telemetry |
| GET | `/api/telemetry` | Recent telemetry list |
| GET | `/api/alerts` | Alert list |

## 5. Compatibility Rules

- Gateway shall reject unsupported versions.
- Reserved fields shall be ignored by current receiver but preserved as zero by sender.
- New packet versions require ICD revision.
