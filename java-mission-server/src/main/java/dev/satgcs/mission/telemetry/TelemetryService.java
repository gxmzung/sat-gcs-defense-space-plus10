package dev.satgcs.mission.telemetry;
import dev.satgcs.mission.alert.*;import org.springframework.beans.factory.annotation.Value;import org.springframework.data.domain.*;import org.springframework.stereotype.Service;import org.springframework.transaction.annotation.Transactional;import java.util.*;
@Service
public class TelemetryService {
 private final TelemetryRepository telemetry; private final AlertRepository alerts;
 private final double batteryWarn,batteryCrit,tempWarn,tempCrit;
 public TelemetryService(TelemetryRepository telemetry, AlertRepository alerts,
  @Value("${satgcs.thresholds.battery-warning-voltage}") double batteryWarn,
  @Value("${satgcs.thresholds.battery-critical-voltage}") double batteryCrit,
  @Value("${satgcs.thresholds.temperature-warning-c}") double tempWarn,
  @Value("${satgcs.thresholds.temperature-critical-c}") double tempCrit){this.telemetry=telemetry;this.alerts=alerts;this.batteryWarn=batteryWarn;this.batteryCrit=batteryCrit;this.tempWarn=tempWarn;this.tempCrit=tempCrit;}
 @Transactional public TelemetryResponse ingest(TelemetryRequest req){
   if(telemetry.existsBySatelliteIdAndSequence(req.satelliteId(), req.sequence())) return telemetry.findTopBySatelliteIdOrderByReceivedAtDesc(req.satelliteId()).map(TelemetryResponse::from).orElseThrow();
   var saved=telemetry.save(TelemetryEntity.of(req)); evaluateAlerts(req); return TelemetryResponse.from(saved);
 }
 @Transactional(readOnly=true) public Optional<TelemetryResponse> latest(String satelliteId){ return telemetry.findTopBySatelliteIdOrderByReceivedAtDesc(satelliteId).map(TelemetryResponse::from); }
 @Transactional(readOnly=true) public Page<TelemetryResponse> history(String satelliteId, int page, int size){ return telemetry.findBySatelliteIdOrderByReceivedAtDesc(satelliteId, PageRequest.of(page, Math.min(size,200))).map(TelemetryResponse::from); }
 @Transactional(readOnly=true) public List<AlertEntity> latestAlerts(){ return alerts.findTop50ByOrderByCreatedAtDesc(); }
 private void evaluateAlerts(TelemetryRequest r){
   if(r.batteryVoltage() <= batteryCrit) alerts.save(new AlertEntity(r.satelliteId(), AlertSeverity.CRITICAL,"BATTERY_CRITICAL","Battery voltage critical: "+r.batteryVoltage(),r.sequence()));
   else if(r.batteryVoltage() <= batteryWarn) alerts.save(new AlertEntity(r.satelliteId(), AlertSeverity.WARNING,"BATTERY_LOW","Battery voltage low: "+r.batteryVoltage(),r.sequence()));
   if(r.temperature() >= tempCrit) alerts.save(new AlertEntity(r.satelliteId(), AlertSeverity.CRITICAL,"TEMP_CRITICAL","Temperature critical: "+r.temperature(),r.sequence()));
   else if(r.temperature() >= tempWarn) alerts.save(new AlertEntity(r.satelliteId(), AlertSeverity.WARNING,"TEMP_HIGH","Temperature high: "+r.temperature(),r.sequence()));
 }
}
