# Architecture

## Overview

The project models a removable battery dock that must validate a pack before it
can connect a high-current DC path. The controller checks pack presence,
alignment, authentication, voltage delta, and dock temperature before deciding
whether to precharge, close the main path, release, or latch a fault.

## Modules

- `battery_swap_dock_controller.c`: state machine and fault-latch behavior
- `precharge_monitor.c`: voltage-delta measurement and precharge thresholds
- `dock_policy.c`: output policy for latch, precharge, main path, cooling, and progress
- `main.c`: deterministic scenario runner used for portfolio snapshots
- `tests/test_battery_swap_dock.c`: nominal and fault-path regression tests

## State Flow

1. `IDLE`: wait for a pack and enable request.
2. `VALIDATE`: confirm alignment, authentication, and acceptable bus delta.
3. `PRECHARGE`: close the latch, energize precharge, and wait for the delta to collapse.
4. `DOCKED`: close the main path and hold the pack ready for load transfer.
5. `RELEASE`: open the path and unlatch the pack for removal.
6. `FAULT`: latch faults until the slot is empty and a reset request arrives.

## Safety Decisions

- Misalignment or failed pack authentication prevents energizing the dock.
- Large voltage mismatch is rejected before the main path can close.
- Precharge timeout forces a latched fault instead of repeated uncontrolled retries.
- Overtemperature opens the dock immediately and enables active cooling.
