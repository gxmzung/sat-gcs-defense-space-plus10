package dev.satgcs.mission.telemetry;
import java.time.Instant;
public record TelemetryResponse(Long id,String satelliteId,long sequence,long timestampMs,double batteryVoltage,double temperature,double roll,double pitch,double yaw,double lat,double lon,TelemetryStatus status,Instant receivedAt) {
  public static TelemetryResponse from(TelemetryEntity e){ return new TelemetryResponse(e.getId(),e.getSatelliteId(),e.getSequence(),e.getTimestampMs(),e.getBatteryVoltage(),e.getTemperature(),e.getRoll(),e.getPitch(),e.getYaw(),e.getLat(),e.getLon(),e.getStatus(),e.getReceivedAt()); }
}
