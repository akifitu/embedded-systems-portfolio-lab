# Motor Condition Monitor

Host-buildable firmware model for predictive maintenance on an industrial motor.

Docs:
- [Implementation Plan](docs/IMPLEMENTATION_PLAN.md)
- [Architecture Notes](docs/ARCHITECTURE.md)

## Features

- Windowed vibration analysis
- Integer-friendly RMS, peak-to-peak, and jerk features
- Fault classification for imbalance, bearing wear, stall, and overtemperature
- Event log ring buffer for state transitions

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=imbalance fault=IMBALANCE sev=WARNING rms=636 p2p=1800 jerk=217 current=5200 temp=38.0C events=1
phase=bearing fault=BEARING_WEAR sev=WARNING rms=740 p2p=2300 jerk=1262 current=5600 temp=41.0C events=2
phase=stall fault=STALL sev=CRITICAL rms=39 p2p=120 jerk=18 current=9800 temp=44.0C events=3
```

## Future Extensions

- DMA-fed accelerometer capture on STM32 or ESP32
- Flash-backed fault history
- Real FFT pipeline with CMSIS-DSP
- CAN or MQTT fault uplink
