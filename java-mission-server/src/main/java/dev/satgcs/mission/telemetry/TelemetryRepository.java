package dev.satgcs.mission.telemetry;
import org.springframework.data.domain.*;import org.springframework.data.jpa.repository.*;import java.util.*;
public interface TelemetryRepository extends JpaRepository<TelemetryEntity, Long> {
 Optional<TelemetryEntity> findTopBySatelliteIdOrderByReceivedAtDesc(String satelliteId);
 Page<TelemetryEntity> findBySatelliteIdOrderByReceivedAtDesc(String satelliteId, Pageable pageable);
 boolean existsBySatelliteIdAndSequence(String satelliteId,long sequence);
}
