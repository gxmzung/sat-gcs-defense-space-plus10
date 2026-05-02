import argparse, json, time, requests
p=argparse.ArgumentParser(); p.add_argument('--api',default='http://localhost:8080'); p.add_argument('--key',default='dev-api-key'); p.add_argument('--file',default='sample_telemetry.jsonl'); p.add_argument('--delay',type=float,default=0.5); args=p.parse_args()
for line in open(args.file,encoding='utf-8'):
    payload=json.loads(line); r=requests.post(args.api+'/api/telemetry',json=payload,headers={'X-API-Key':args.key},timeout=3)
    print(payload['sequence'], r.status_code, r.text[:80]); time.sleep(args.delay)
