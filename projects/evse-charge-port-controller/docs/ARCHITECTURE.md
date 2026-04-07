# Architecture

## Overview

The project models an EVSE control loop that runs once per service frame. Each step:

1. Samples control-pilot voltage, PWM duty, cable limit, temperature, GFCI, and contactor feedback
2. Decodes pilot state and advertised current capability
3. Applies current limiting from cable rating and thermal conditions
4. Sequences contactor close and charging enable when the vehicle requests power
5. Trips to fault on GFCI, pilot, temperature, or contactor anomalies and waits through cooldown before recovery

## Modules

- `evse_types.*`: enums, string helpers, and shared I/O structures
- `pilot_decoder.*`: control-pilot state decode and PWM-current conversion
- `current_policy.*`: current-limit clamping and thermal derating
- `evse_controller.*`: idle, connected, arming, charging, fault, and cooldown state machine
- `main.c`: deterministic charging scenarios for portfolio review

## State Model

- `IDLE`: no vehicle connected, contactor open
- `CONNECTED`: vehicle detected but not yet requesting charge
- `ARMING`: vehicle requests charge and the EVSE is closing the contactor
- `CHARGING`: charging is enabled and current is being advertised
- `FAULT`: a protection condition opened the contactor immediately
- `COOLDOWN`: fault cleared electrically but the EVSE is holding off re-enable

## Fault Model

- `pilot_fault`: control pilot is in an invalid state
- `gfci`: residual-current protection tripped
- `overtemp`: enclosure or cable temperature exceeded a hard limit
- `contactor_fault`: feedback does not match the expected contactor state
