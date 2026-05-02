package dev.satgcs.mission.telemetry;
import jakarta.persistence.*;import java.time.Instant;
@Entity @Table(name="telemetry")
public class TelemetryEntity {
 @Id @GeneratedValue(strategy=GenerationType.IDENTITY) private Long id;
 @Column(nullable=false) private String satelliteId; @Column(nullable=false) private long sequence; @Column(nullable=false) private long timestampMs;
 private double batteryVoltage, temperature, roll, pitch, yaw, lat, lon;
 @Enumerated(EnumType.STRING) private TelemetryStatus status;
 @Column(nullable=false) private Instant receivedAt=Instant.now();
 public static TelemetryEntity of(TelemetryRequest r){ var e=new TelemetryEntity(); e.satelliteId=r.satelliteId(); e.sequence=r.sequence(); e.timestampMs=r.timestampMs(); e.batteryVoltage=r.batteryVoltage(); e.temperature=r.temperature(); e.roll=r.roll(); e.pitch=r.pitch(); e.yaw=r.yaw(); e.lat=r.lat(); e.lon=r.lon(); e.status=r.status(); return e; }
 public Long getId(){return id;} public String getSatelliteId(){return satelliteId;} public long getSequence(){return sequence;} public long getTimestampMs(){return timestampMs;} public double getBatteryVoltage(){return batteryVoltage;} public double getTemperature(){return temperature;} public double getRoll(){return roll;} public double getPitch(){return pitch;} public double getYaw(){return yaw;} public double getLat(){return lat;} public double getLon(){return lon;} public TelemetryStatus getStatus(){return status;} public Instant getReceivedAt(){return receivedAt;}
}
