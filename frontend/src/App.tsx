import React, {useEffect, useMemo, useState} from 'react';
import { createRoot } from 'react-dom/client';
import { AlertTriangle, Activity, Battery, Thermometer, Radio, Rocket, Orbit, TerminalSquare, MapPinned } from 'lucide-react';
import { LineChart, Line, XAxis, YAxis, Tooltip, CartesianGrid, ResponsiveContainer } from 'recharts';
import './style.css';

type Telemetry={id:number,satelliteId:string,sequence:number,timestampMs:number,batteryVoltage:number,temperature:number,roll:number,pitch:number,yaw:number,lat:number,lon:number,status:string,receivedAt:string};
type Alert={id:number,satelliteId:string,severity:string,code:string,message:string,telemetrySequence:number,createdAt:string,acknowledged:boolean};
type LaunchPhase={key:string,label:string,at:number,description:string};
type ConsoleEvent={time:string,level:'INFO'|'WARN'|'ERROR'|'EVENT',message:string};

const API=import.meta.env.VITE_API_BASE_URL || 'http://localhost:8080';
const GS={name:'Daejeon Ground Station', lat:36.3504, lon:127.3845};
const PHASES:LaunchPhase[]=[
 {key:'T_MINUS_10',label:'T-10',at:0,description:'Countdown sequence armed'},
 {key:'LIFTOFF',label:'LIFTOFF',at:10,description:'Main engine ignition and liftoff'},
 {key:'BOOST_ASCENT',label:'BOOST',at:22,description:'Booster ascent and telemetry lock attempt'},
 {key:'STAGE_SEP',label:'STAGE SEP',at:45,description:'Stage separation event'},
 {key:'ORBIT_INSERTION',label:'ORBIT',at:70,description:'Orbit injection burn complete'},
 {key:'LINK_ESTABLISHED',label:'TM LINK',at:88,description:'Telemetry link established with ground gateway'}
];

async function getJson<T>(path:string):Promise<T>{const r=await fetch(API+path); if(!r.ok) throw new Error(`${r.status}`); return r.json();}
function Card({title,value,icon}:{title:string,value:string,icon:React.ReactNode}){return <div className="card"><div className="card-title">{icon}<span>{title}</span></div><div className="card-value">{value}</div></div>}
function clamp(n:number,min:number,max:number){return Math.max(min,Math.min(max,n));}
function project(lon:number,lat:number){return {x:((lon+180)/360)*100,y:((90-lat)/180)*100};}
function inView(lat:number,lon:number){const dLat=lat-GS.lat; const dLon=(lon-GS.lon)*Math.cos((GS.lat*Math.PI)/180); const distance=Math.sqrt(dLat*dLat+dLon*dLon); return distance<30;}
function phaseAt(elapsed:number){return PHASES.reduce((prev,p)=>elapsed>=p.at?p:prev,PHASES[0]);}

function LaunchSequence({latest,linkOk}:{latest:Telemetry|null,linkOk:boolean}){
 const [start]=useState(()=>Date.now());
 const [now,setNow]=useState(Date.now());
 useEffect(()=>{const id=setInterval(()=>setNow(Date.now()),1000); return()=>clearInterval(id);},[]);
 const elapsed=Math.floor((now-start)/1000);
 const phase=latest?{key:'LINK_ESTABLISHED',label:'TM LINK',at:88,description:'Live telemetry received'}:phaseAt(elapsed);
 const progress=latest?100:clamp((elapsed/88)*100,0,100);
 return <section className="panel launch-panel">
  <div className="panel-head"><div><h2>Launch Sequence</h2><p>Countdown → liftoff → orbit injection → telemetry lock</p></div><Rocket/></div>
  <div className="launch-visual">
   <div className="earth-horizon"></div>
   <div className="rocket" style={{bottom:`${18+progress*0.58}%`, left:`${18+progress*0.48}%`, transform:`rotate(${clamp(progress*0.28,0,28)}deg)`}}>🚀</div>
   <div className="orbit-arc"></div>
  </div>
  <div className="phase-status"><strong>{phase.label}</strong><span>{phase.description}</span><em className={linkOk?'ok':'warn'}>{linkOk?'LINK OK':'WAITING TM'}</em></div>
  <div className="phase-track">{PHASES.map(p=><div key={p.key} className={`phase ${elapsed>=p.at||latest?'active':''}`}><span>{p.label}</span></div>)}</div>
 </section>
}

function OrbitMap({latest,history}:{latest:Telemetry|null,history:Telemetry[]}){
 const sat=latest?project(latest.lon,latest.lat):null; const gs=project(GS.lon,GS.lat);
 const path=history.map(t=>project(t.lon,t.lat)).map((p,i)=>(i===0?'M':'L')+`${p.x.toFixed(2)} ${p.y.toFixed(2)}`).join(' ');
 const visible=latest?inView(latest.lat,latest.lon):false;
 return <section className="panel map-panel">
  <div className="panel-head"><div><h2>2D Orbit & Ground Track</h2><p>Satellite position, recent ground track, ground station visibility</p></div><MapPinned/></div>
  <svg viewBox="0 0 100 50" className="world-map" role="img" aria-label="Satellite orbit map">
   <rect x="0" y="0" width="100" height="50" rx="3"/>
   <path className="gridline" d="M0 25H100M50 0V50M25 0V50M75 0V50M0 12.5H100M0 37.5H100"/>
   <path className="land" d="M11 17 C17 8 28 9 32 16 C36 24 28 29 19 27 C11 25 6 23 11 17 Z M41 12 C50 6 60 9 64 17 C70 29 55 32 47 27 C40 23 35 17 41 12 Z M66 31 C73 24 87 26 91 34 C94 42 82 45 72 41 C64 38 61 35 66 31 Z"/>
   {path&&<path className="track" d={path}/>} 
   <circle className="ground" cx={gs.x} cy={gs.y} r="1.4"/><text x={gs.x+2} y={gs.y-1} className="map-label">Daejeon GS</text>
   {sat&&<><circle className="sat" cx={sat.x} cy={sat.y} r="1.3"/><circle className={visible?'coverage on':'coverage'} cx={sat.x} cy={sat.y} r="5"/><text x={sat.x+2} y={sat.y+1} className="map-label">SAT</text></>}
  </svg>
  <div className="map-meta"><span>Ground Station: {GS.lat.toFixed(4)}, {GS.lon.toFixed(4)}</span><span className={visible?'ok':'warn'}>{visible?'PASS VISIBLE':'OUT OF VIEW'}</span></div>
 </section>
}

function OperatorConsole({latest,alerts,err}:{latest:Telemetry|null,alerts:Alert[],err:string}){
 const events=useMemo<ConsoleEvent[]>(()=>{
  const base:ConsoleEvent[]=[
   {time:new Date().toLocaleTimeString(),level:'INFO',message:'Operator console online'},
   {time:new Date().toLocaleTimeString(),level:'EVENT',message:'LAUNCH_INIT sequence monitored'},
  ];
  if(err) base.push({time:new Date().toLocaleTimeString(),level:'WARN',message:'Mission server heartbeat unavailable: '+err});
  if(latest){
   base.push({time:new Date(latest.receivedAt).toLocaleTimeString(),level:'EVENT',message:`TELEMETRY_LOCK ${latest.satelliteId} seq=${latest.sequence}`});
   base.push({time:new Date(latest.receivedAt).toLocaleTimeString(),level:'INFO',message:`ATT roll=${latest.roll.toFixed(2)} pitch=${latest.pitch.toFixed(2)} yaw=${latest.yaw.toFixed(2)}`});
  }
  alerts.slice(0,5).forEach(a=>base.push({time:new Date(a.createdAt).toLocaleTimeString(),level:a.severity==='CRITICAL'?'ERROR':'WARN',message:`${a.code}: ${a.message}`}));
  return base.slice(-10).reverse();
 },[latest,alerts,err]);
 return <section className="panel console-panel">
  <div className="panel-head"><div><h2>Mission Console Log</h2><p>Event stream used for operator situational awareness</p></div><TerminalSquare/></div>
  <div className="console-lines">{events.map((e,i)=><div className="console-line" key={i}><span>{e.time}</span><b className={e.level.toLowerCase()}>{e.level}</b><code>{e.message}</code></div>)}</div>
 </section>
}

function App(){
 const [latest,setLatest]=useState<Telemetry|null>(null); const [history,setHistory]=useState<Telemetry[]>([]); const [alerts,setAlerts]=useState<Alert[]>([]); const [err,setErr]=useState<string>('');
 useEffect(()=>{ const load=async()=>{try{setLatest(await getJson<Telemetry>('/api/telemetry/latest')); const h=await getJson<any>('/api/telemetry/history?size=30'); setHistory((h.content||[]).reverse()); setAlerts(await getJson<Alert[]>('/api/alerts')); setErr('');}catch(e:any){setErr('Mission server not ready: '+e.message)}}; load(); const id=setInterval(load,2000); return()=>clearInterval(id);},[]);
 const linkOk=Boolean(latest)&&!err;
 return <main><header><h1>SAT-GCS Operator Dashboard</h1><p>Defense/space-style mini ground control system: launch state, telemetry, orbit view, and operator console</p></header>{err&&<div className="error">{err}</div>}
 <section className="grid"><Card title="Status" value={latest?.status??'NO DATA'} icon={<Activity/>}/><Card title="Telemetry Link" value={linkOk?'LOCKED':'WAIT'} icon={<Radio/>}/><Card title="Battery" value={latest?latest.batteryVoltage.toFixed(3)+' V':'-'} icon={<Battery/>}/><Card title="Temperature" value={latest?latest.temperature.toFixed(2)+' °C':'-'} icon={<Thermometer/>}/></section>
 <section className="viz-grid"><LaunchSequence latest={latest} linkOk={linkOk}/><OrbitMap latest={latest} history={history}/></section>
 <OperatorConsole latest={latest} alerts={alerts} err={err}/>
 <section className="panel"><h2>Telemetry Trend</h2><ResponsiveContainer width="100%" height={300}><LineChart data={history}><CartesianGrid strokeDasharray="3 3"/><XAxis dataKey="sequence"/><YAxis/><Tooltip/><Line type="monotone" dataKey="batteryVoltage" dot={false}/><Line type="monotone" dataKey="temperature" dot={false}/></LineChart></ResponsiveContainer></section>
 <section className="panel"><h2>Latest Telemetry</h2><pre>{JSON.stringify(latest,null,2)}</pre></section>
 <section className="panel"><h2>Recent Alerts</h2><table><thead><tr><th>Time</th><th>Severity</th><th>Code</th><th>Message</th></tr></thead><tbody>{alerts.map(a=><tr key={a.id}><td>{new Date(a.createdAt).toLocaleString()}</td><td>{a.severity}</td><td>{a.code}</td><td>{a.message}</td></tr>)}</tbody></table></section>
 </main>
}
createRoot(document.getElementById('root')!).render(<App/>);
