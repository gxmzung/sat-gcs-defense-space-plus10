import json, jsonschema
schema={"type":"object","required":["satelliteId","sequence","timestampMs","batteryVoltage","temperature","roll","pitch","yaw","lat","lon","status"]}
def test_sample_payloads():
    for line in open('sample_telemetry.jsonl',encoding='utf-8'):
        jsonschema.validate(json.loads(line), schema)
