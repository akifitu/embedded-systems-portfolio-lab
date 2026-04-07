# UAV Failsafe Controller

Portable C model of a flight-controller failsafe supervisor. The project watches
GPS health, RC link continuity, geofence limits, and battery return margin to
decide between mission continuation, return-to-home, emergency landing, and
motor cutoff after touchdown.

## Why It Matters

- Shows safety-critical state-machine logic beyond sensor reading or protocol parsing
- Demonstrates how embedded autopilots degrade from nominal flight into RTL and landing
- Maps directly to STM32, PX4, ArduPilot, and custom UAV safety-controller work

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=launch state=MISSION cmd=PROCEED reason=NONE battery=92 reserve=73 budget=19 link=0 fence=IN health=GREEN
phase=geofence state=RTL cmd=RETURN_HOME reason=GEOFENCE battery=78 reserve=36 budget=42 link=0 fence=OUT health=YELLOW
phase=link_loss state=RTL cmd=RETURN_HOME reason=LINK_LOSS battery=68 reserve=38 budget=30 link=3 fence=IN health=YELLOW
phase=nav_loss state=LAND cmd=DESCEND reason=NAV_LOSS battery=61 reserve=33 budget=28 link=2 fence=IN health=RED
phase=critical_battery state=LAND cmd=DESCEND reason=CRITICAL_BATTERY battery=9 reserve=0 budget=16 link=0 fence=IN health=RED
phase=touchdown state=DISARMED cmd=CUT_MOTORS reason=NONE battery=8 reserve=0 budget=14 link=0 fence=IN health=RED
```

## Implementation Notes

- `reserve_model.c` estimates how much battery budget is needed to come home
- `geofence.c` tracks whether the vehicle is outside its allowed radius
- `uav_failsafe_controller.c` applies mission, RTL, landing, and disarm logic

## Hardware Path

Natural next step: wire the controller to GPS, RC RSSI, battery telemetry, and
barometric altitude on STM32, RP2040, or a PX4-class autopilot board.
