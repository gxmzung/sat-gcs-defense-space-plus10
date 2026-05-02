package dev.satgcs.mission.alert;
import jakarta.persistence.*;import java.time.Instant;
@Entity @Table(name="alert")
public class AlertEntity {
 @Id @GeneratedValue(strategy=GenerationType.IDENTITY) private Long id;
 @Column(nullable=false) private String satelliteId; @Enumerated(EnumType.STRING) private AlertSeverity severity;
 @Column(nullable=false) private String code; @Column(nullable=false) private String message; @Column(nullable=false) private long telemetrySequence;
 @Column(nullable=false) private Instant createdAt=Instant.now(); private boolean acknowledged=false;
 protected AlertEntity(){}
 public AlertEntity(String satelliteId, AlertSeverity severity, String code, String message, long telemetrySequence){this.satelliteId=satelliteId;this.severity=severity;this.code=code;this.message=message;this.telemetrySequence=telemetrySequence;}
 public Long getId(){return id;} public String getSatelliteId(){return satelliteId;} public AlertSeverity getSeverity(){return severity;} public String getCode(){return code;} public String getMessage(){return message;} public long getTelemetrySequence(){return telemetrySequence;} public Instant getCreatedAt(){return createdAt;} public boolean isAcknowledged(){return acknowledged;} public void acknowledge(){this.acknowledged=true;}
}
