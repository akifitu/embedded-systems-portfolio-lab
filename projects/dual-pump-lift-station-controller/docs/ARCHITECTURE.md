# Architecture

## Overview

The project models a dual-pump lift-station supervisor that runs once per level
update. Each step:

1. Reads wet-well level, inflow, pump readiness, seal status, and reset input
2. Applies start/stop/high-high/overflow thresholds
3. Selects a lead or backup pump according to the alternation policy
4. Starts the assist pump at high-high level when both pumps are available
5. Trips to fault on overflow or seal failures and waits for reset-based recovery

## Modules

- `lift_station_types.*`: enums, names, and shared input/output structures
- `level_policy.*`: threshold helpers for start, stop, assist, and overflow
- `pump_arbiter.*`: lead/lag selection and backup pump fallback
- `lift_station_controller.*`: main state machine and alternation logic
- `main.c`: deterministic wet-well scenarios for portfolio review

## State Model

- `IDLE`: no pumps running
- `DRAIN_LEAD`: one pump is draining the wet well
- `DRAIN_ASSIST`: both pumps run during a high-high event
- `FAULT`: one or more conditions require operator reset

## Fault Model

- `OVERFLOW`: wet-well level exceeded the emergency threshold
- `NO_PUMP_AVAILABLE`: neither pump can run when pumping is required
- `PUMP_A_SEAL` and `PUMP_B_SEAL`: seal leakage blocks the affected pump
