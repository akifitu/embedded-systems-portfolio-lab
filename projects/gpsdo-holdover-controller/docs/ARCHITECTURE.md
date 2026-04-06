# Architecture

## Overview

The project models a GPSDO controller that runs once per second. Each step:

1. Reads PPS validity, measured phase error, and oscillator temperature
2. Updates the trim DAC while PPS is valid and phase error is plausible
3. Learns a simple temperature drift coefficient while locked
4. Switches to holdover when PPS is missing
5. Expands uncertainty until PPS returns or a fault is declared

## Modules

- `gpsdo_types.*`: enums, quality labels, and shared I/O structures
- `drift_model.*`: temperature-to-trim bias and uncertainty estimation
- `gpsdo_controller.*`: acquire, track, holdover, relock, and fault logic
- `main.c`: deterministic timing scenarios

## State Model

- `ACQUIRE`: PPS is present but lock confidence is still building
- `TRACKING`: PPS is valid and the oscillator is disciplined
- `HOLDOVER`: PPS is absent, trim is predicted from recent lock history
- `FAULT`: PPS is implausible or timing error is too large

## Fault Model

- `bad_pps`: phase error is beyond a safe sanity threshold
