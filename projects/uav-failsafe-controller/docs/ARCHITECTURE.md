# Architecture

## Overview

The project models a UAV failsafe supervisor that runs once per control frame.
Each step:

1. Reads arming state, RC link status, GPS health, battery, distance to home,
   altitude, geofence radius, and wind estimate
2. Computes a return-to-home battery budget and remaining reserve margin
3. Checks whether the aircraft is outside the geofence
4. Promotes the vehicle from mission flight to RTL or emergency landing
5. Disarms after touchdown when the landing sequence completes

## Modules

- `failsafe_types.*`: enums, string helpers, and shared input/output structures
- `reserve_model.*`: battery budget estimate for a safe return-to-home
- `geofence.*`: in-bounds versus out-of-bounds radius evaluation
- `uav_failsafe_controller.*`: mission, RTL, landing, and disarm state machine
- `main.c`: deterministic flight-safety scenarios

## State Model

- `STANDBY`: vehicle is not armed or arming checks are incomplete
- `MISSION`: flight continues normally
- `RTL`: the aircraft must return home because a limit or degradation was detected
- `LAND`: the aircraft must descend immediately
- `DISARMED`: landing is complete and motors are cut

## Fault Model

- `arming_block`: GPS, satellite count, or battery level is insufficient for launch
- `geofence`: the aircraft moved beyond the allowed radius
- `link_loss`: RC link disappeared for too many control frames
- `return_margin`: battery reserve is no longer safe for continued mission flight
- `nav_loss`: link and navigation confidence are both degraded
- `critical_battery`: remaining battery requires immediate landing
