package dev.satgcs.mission.telemetry;
import jakarta.validation.constraints.*;
public record TelemetryRequest(
  @NotBlank String satelliteId,
  @PositiveOrZero long sequence,
  @Positive long timestampMs,
  @DecimalMin("0.0") @DecimalMax("20.0") double batteryVoltage,
  @DecimalMin("-80.0") @DecimalMax("120.0") double temperature,
  @DecimalMin("-180.0") @DecimalMax("180.0") double roll,
  @DecimalMin("-180.0") @DecimalMax("180.0") double pitch,
  @DecimalMin("-360.0") @DecimalMax("360.0") double yaw,
  @DecimalMin("-90.0") @DecimalMax("90.0") double lat,
  @DecimalMin("-180.0") @DecimalMax("180.0") double lon,
  @NotNull TelemetryStatus status
) {}
