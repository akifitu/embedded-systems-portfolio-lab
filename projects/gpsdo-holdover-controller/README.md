# GPSDO Holdover Controller

Portable C model of a GPS-disciplined oscillator controller. The project locks
an oscillator trim DAC to a valid PPS reference, estimates temperature-related
drift, enters holdover when PPS disappears, and expands timing uncertainty until
GPS lock returns or a fault is detected.

## Why It Matters

- Shows embedded timing and control logic instead of only protocol handling
- Demonstrates lock acquisition, drift compensation, and holdover risk tracking
- Maps directly to precision timebase products with PPS capture and DAC trim

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=warmup state=ACQUIRE trim=2041 phase=180ns temp=24.0C pps=YES uncertainty=0ns quality=SEARCH
phase=locked state=TRACKING trim=2038 phase=12ns temp=24.2C pps=YES uncertainty=0ns quality=LOCKED
phase=holdover_30s state=HOLDOVER trim=2058 phase=28ns temp=26.0C pps=NO uncertainty=95ns quality=HOLDOVER
phase=holdover_2m state=HOLDOVER trim=2060 phase=74ns temp=27.5C pps=NO uncertainty=410ns quality=HOLDOVER
phase=relock state=TRACKING trim=2055 phase=6ns temp=25.0C pps=YES uncertainty=0ns quality=LOCKED
phase=bad_pps state=FAULT trim=2048 phase=8500ns temp=25.0C pps=YES uncertainty=0ns quality=FAULT
```

## Implementation Notes

- `drift_model.c` estimates trim drift from temperature change
- `gpsdo_controller.c` handles acquire, track, holdover, relock, and fault logic
- `main.c` runs deterministic PPS and holdover scenarios for portfolio review

## Hardware Path

Natural next step: pair the controller with PPS capture hardware, a DAC-driven
VCXO or OCXO, and temperature telemetry on STM32, RP2040, or an FPGA companion.
