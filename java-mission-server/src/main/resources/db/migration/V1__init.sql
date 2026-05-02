create table telemetry (
  id bigserial primary key,
  satellite_id varchar(64) not null,
  sequence bigint not null,
  timestamp_ms bigint not null,
  battery_voltage double precision not null,
  temperature double precision not null,
  roll double precision not null,
  pitch double precision not null,
  yaw double precision not null,
  lat double precision not null,
  lon double precision not null,
  status varchar(32) not null,
  received_at timestamptz not null default now()
);
create index idx_telemetry_sat_received on telemetry(satellite_id, received_at desc);
create unique index uk_telemetry_sat_seq on telemetry(satellite_id, sequence);

create table alert (
  id bigserial primary key,
  satellite_id varchar(64) not null,
  severity varchar(16) not null,
  code varchar(64) not null,
  message varchar(512) not null,
  telemetry_sequence bigint not null,
  created_at timestamptz not null default now(),
  acknowledged boolean not null default false
);
create index idx_alert_created on alert(created_at desc);
