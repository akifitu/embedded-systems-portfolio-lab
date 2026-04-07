# EVSE Charge Port Controller

Portable C model of an EV supply equipment controller. The project decodes
control-pilot states, advertises current from PWM duty and cable limits,
derates charging current under thermal stress, trips on GFCI or contactor
faults, and enforces a cooldown before re-energizing the port.

## Why It Matters

- Shows EV charging infrastructure logic rather than only generic protocol code
- Demonstrates pilot-state handling, contactor sequencing, and fault lockout behavior
- Maps directly to AC charger, wallbox, and smart-EVSE firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=idle state=IDLE cmd=OPEN_CONTACTOR pilot=A limit=0.0A cool=0 contactor=OPEN fault=NONE
phase=plugged state=CONNECTED cmd=ADVERTISE_CURRENT pilot=B limit=32.0A cool=0 contactor=OPEN fault=NONE
phase=arming state=ARMING cmd=CLOSE_CONTACTOR pilot=C limit=32.0A cool=0 contactor=OPEN fault=NONE
phase=charging state=CHARGING cmd=ENERGIZE_PORT pilot=C limit=32.0A cool=0 contactor=CLOSED fault=NONE
phase=thermal_derate state=CHARGING cmd=ENERGIZE_PORT pilot=C limit=18.0A cool=0 contactor=CLOSED fault=NONE
phase=gfci_trip state=FAULT cmd=OPEN_CONTACTOR pilot=C limit=0.0A cool=3 contactor=OPEN fault=GFCI
phase=cooldown state=COOLDOWN cmd=HOLD_COOLDOWN pilot=B limit=0.0A cool=3 contactor=OPEN fault=GFCI
phase=recover state=CHARGING cmd=ENERGIZE_PORT pilot=C limit=32.0A cool=0 contactor=CLOSED fault=NONE
```

## Implementation Notes

- `pilot_decoder.c` converts control-pilot voltage and PWM duty into pilot state and advertised current
- `current_policy.c` applies cable-limit clamping and thermal current derating
- `evse_controller.c` manages idle, connected, arming, charging, fault, and cooldown logic

## Hardware Path

Natural next step: port the controller to an STM32 or NXP charger MCU with
CP/PP sampling, GFCI input, contactor drive, welded-contactor feedback, and a
mechanical lock actuator.
