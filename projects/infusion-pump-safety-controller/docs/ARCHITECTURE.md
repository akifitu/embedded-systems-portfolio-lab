# Architecture

## Overview

The project models an infusion pump controller that runs once per service
frame. Each step:

1. Reads user requests, door/interlock status, reservoir estimate, and safety inputs
2. Classifies any active shutdown condition
3. Advances programmed dose delivery when therapy is allowed
4. Falls back to KVO after the programmed volume is complete
5. Latches alarm state until the operator performs a safe reset

## Modules

- `pump_types.*`: enums, labels, and shared input/output structures
- `safety_monitor.*`: fault classification helpers
- `dose_engine.*`: therapy progression and completion helpers
- `infusion_pump_controller.*`: priming, infusion, KVO, pause, alarm, and reset logic
- `main.c`: deterministic therapy scenarios for GitHub review

## State Model

- `IDLE`: therapy armed but not currently driving the pump
- `PRIMING`: line prime active, clamp open, therapy dose not yet consumed
- `INFUSING`: programmed therapy rate active
- `PAUSED`: therapy temporarily paused by the operator
- `KVO`: programmed dose complete, low-rate keep-vein-open flow active
- `ALARM`: pump stopped and fault latched until reset

## Fault Model

- `OCCLUSION`: line pressure indicates blocked flow
- `AIR_IN_LINE`: bubble detector tripped
- `DOOR_OPEN`: pump door or cassette interlock is open
- `EMPTY_RESERVOIR`: reservoir estimate is depleted
