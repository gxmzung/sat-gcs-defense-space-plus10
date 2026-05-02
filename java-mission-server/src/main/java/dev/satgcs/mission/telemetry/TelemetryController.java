package dev.satgcs.mission.telemetry;
import dev.satgcs.mission.alert.AlertEntity;import jakarta.validation.Valid;import org.springframework.data.domain.Page;import org.springframework.http.*;import org.springframework.web.bind.annotation.*;import java.util.*;
@RestController @RequestMapping("/api") @CrossOrigin
public class TelemetryController {
 private final TelemetryService service; public TelemetryController(TelemetryService service){this.service=service;}
 @PostMapping("/telemetry") public ResponseEntity<TelemetryResponse> ingest(@RequestBody @Valid TelemetryRequest request){ return ResponseEntity.status(201).body(service.ingest(request)); }
 @GetMapping("/telemetry/latest") public ResponseEntity<TelemetryResponse> latest(@RequestParam(defaultValue="KOMPSAT-SIM-01") String satelliteId){ return service.latest(satelliteId).map(ResponseEntity::ok).orElse(ResponseEntity.notFound().build()); }
 @GetMapping("/telemetry/history") public Page<TelemetryResponse> history(@RequestParam(defaultValue="KOMPSAT-SIM-01") String satelliteId,@RequestParam(defaultValue="0") int page,@RequestParam(defaultValue="50") int size){ return service.history(satelliteId,page,size); }
 @GetMapping("/alerts") public List<AlertEntity> alerts(){ return service.latestAlerts(); }
}
