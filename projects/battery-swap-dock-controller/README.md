# Battery Swap Dock Controller

Portable C model of a hot-swappable battery dock that validates pack alignment,
checks authentication, precharges the DC bus, closes the main path, and safely
releases the pack on command or fault.

## Why It Matters

- Shows high-current interconnect safety instead of generic sensor examples
- Demonstrates pack validation, precharge sequencing, latch control, and lockout
- Maps directly to EV, robotics, AGV, and modular power-platform firmware work

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=slot_idle state=IDLE cmd=OPEN_DOCK fault=NONE pack=ABSENT latch=OPEN main=OPEN precharge=OFF fan=OFF delta=0mV progress=0
phase=validate_pack state=VALIDATE cmd=VERIFY_PACK fault=NONE pack=PRESENT latch=OPEN main=OPEN precharge=OFF fan=OFF delta=2200mV progress=25
phase=precharge_ramp state=PRECHARGE cmd=PRECHARGE_BUS fault=NONE pack=PRESENT latch=CLOSED main=OPEN precharge=ON fan=OFF delta=900mV progress=65
phase=docked_ready state=DOCKED cmd=CLOSE_MAIN_PATH fault=NONE pack=PRESENT latch=CLOSED main=CLOSED precharge=OFF fan=OFF delta=120mV progress=100
phase=release_cycle state=RELEASE cmd=RELEASE_PACK fault=NONE pack=PRESENT latch=OPEN main=OPEN precharge=OFF fan=OFF delta=0mV progress=0
phase=thermal_fault state=FAULT cmd=LATCH_FAULT fault=OVER_TEMP pack=PRESENT latch=OPEN main=OPEN precharge=OFF fan=ON delta=1600mV progress=0
phase=reset_ready state=IDLE cmd=RESET_DOCK fault=NONE pack=ABSENT latch=OPEN main=OPEN precharge=OFF fan=OFF delta=0mV progress=0
```

## Implementation Notes

- `precharge_monitor.c` computes pack-to-bus voltage delta and ready thresholds
- `dock_policy.c` maps dock states into latch, precharge, main-path, and fan outputs
- `battery_swap_dock_controller.c` handles validation, precharge timeout, release, fault latch, and reset flow

## Hardware Path

Natural next step: port the controller to STM32, NXP, or an automotive or
robotics power board with latch actuator drivers, precharge resistor control,
main contactors, alignment switches, pack authentication over CAN or 1-Wire,
and connector temperature sensing.
