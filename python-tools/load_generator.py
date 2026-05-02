import argparse, random, time, requests
p=argparse.ArgumentParser(); p.add_argument('--api',default='http://localhost:8080'); p.add_argument('--key',default='dev-api-key'); p.add_argument('--n',type=int,default=100); args=p.parse_args()
for i in range(args.n):
    payload={"satelliteId":"KOMPSAT-SIM-01","sequence":100000+i,"timestampMs":int(time.time()*1000),"batteryVoltage":round(random.uniform(6.8,7.6),3),"temperature":round(random.uniform(28,66),2),"roll":0,"pitch":0,"yaw":90,"lat":36.35,"lon":127.38,"status":"NORMAL"}
    if payload['batteryVoltage']<6.9 or payload['temperature']>60: payload['status']='CRITICAL'
    requests.post(args.api+'/api/telemetry',json=payload,headers={'X-API-Key':args.key},timeout=2)
print('done')
