package dev.satgcs.mission;
import dev.satgcs.mission.telemetry.*;import org.junit.jupiter.api.Test;import org.springframework.beans.factory.annotation.Autowired;import org.springframework.boot.test.context.SpringBootTest;import org.springframework.test.context.TestPropertySource;import static org.assertj.core.api.Assertions.*;
@SpringBootTest
@TestPropertySource(properties={"spring.datasource.url=jdbc:h2:mem:testdb;MODE=PostgreSQL;DB_CLOSE_DELAY=-1","spring.flyway.enabled=false","spring.jpa.hibernate.ddl-auto=create-drop"})
class TelemetryServiceTest {
 @Autowired TelemetryService service;
 @Test void ingestAndFindLatest(){ var r=new TelemetryRequest("KOMPSAT-SIM-01",1,123,7.4,31.0,0,0,0,36,127,TelemetryStatus.NORMAL); var saved=service.ingest(r); assertThat(saved.sequence()).isEqualTo(1); assertThat(service.latest("KOMPSAT-SIM-01")).isPresent(); }
}
