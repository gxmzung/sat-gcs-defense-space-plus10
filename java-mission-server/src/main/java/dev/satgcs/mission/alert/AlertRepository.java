package dev.satgcs.mission.alert;
import org.springframework.data.jpa.repository.*;import java.util.*;
public interface AlertRepository extends JpaRepository<AlertEntity, Long> {
 List<AlertEntity> findTop50ByOrderByCreatedAtDesc();
}
