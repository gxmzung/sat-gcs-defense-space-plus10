# Telemetry Binary Protocol v1

| Field | Size | Type | Description |
|---|---:|---|---|
| magic | 2 | uint16 | `0x5347` = SG |
| version | 1 | uint8 | protocol version |
| flags | 1 | uint8 | bit flags |
| sequence | 4 | uint32 | packet sequence |
| satellite_id | 16 | char[16] | null-padded ASCII |
| unix_ms | 8 | int64 | epoch milliseconds |
| battery_mv | 2 | uint16 | millivolts |
| temperature_centi_c | 2 | int16 | celsius * 100 |
| roll_mdeg | 4 | int32 | millidegree |
| pitch_mdeg | 4 | int32 | millidegree |
| yaw_mdeg | 4 | int32 | millidegree |
| lat_e7 | 4 | int32 | latitude * 1e7 |
| lon_e7 | 4 | int32 | longitude * 1e7 |
| status | 1 | uint8 | 0 normal, 1 warning, 2 critical |
| reserved | 3 | bytes | reserved |
| crc32 | 4 | uint32 | CRC over all previous bytes |

Total: 64 bytes.
