# Visualization Notes

## Purpose

The visualization layer is designed to make the ground control system easier to understand in a portfolio and interview setting. It shows the transition from launch event monitoring to live telemetry operations.

## Components

### Launch Sequence

The launch sequence panel shows a simplified mission timeline:

1. Countdown armed
2. Liftoff
3. Booster ascent
4. Stage separation
5. Orbit insertion
6. Telemetry link established

Once live telemetry is available from the mission server, the phase is forced to `TM LINK` to indicate that the system is now in live operations mode.

### 2D Orbit & Ground Track

The map panel projects latitude and longitude into a simplified 2D equirectangular view. It displays:

- Current satellite position
- Recent telemetry path
- Daejeon ground station marker
- Simplified pass visibility status

This is not a certified orbit propagator. It is a ground-track visualization for operator awareness.

### Mission Console Log

The console panel presents event-style logs for:

- Console readiness
- Launch init
- Telemetry lock
- Attitude values
- Warning/critical alerts

This helps explain how operators would monitor system state transitions and anomalies.

## Limitations

- No real orbital propagator such as SGP4 is included.
- No real launch vehicle dynamics are modeled.
- Visibility is calculated using a simplified latitude/longitude distance rule.
- Visualization is not suitable for operational mission planning.

## Interview Positioning

Recommended phrasing:

> The visualization layer is intentionally simplified. The goal is not to claim real launch simulation accuracy, but to show how telemetry, launch events, ground-track awareness, and operator logs can be integrated into a single ground control dashboard.
