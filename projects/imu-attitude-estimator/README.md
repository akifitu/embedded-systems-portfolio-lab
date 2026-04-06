# IMU Attitude Estimator

Host-buildable IMU sensor-fusion model for embedded orientation tracking.

Docs:
- [Implementation Plan](docs/IMPLEMENTATION_PLAN.md)
- [Architecture Notes](docs/ARCHITECTURE.md)

## Features

- Roll and pitch estimation from gyro plus accelerometer fusion
- Dynamic complementary filter weighting from acceleration magnitude
- Motion-state classification for stable, tracking, vibration, and freefall
- Deterministic phase replay suitable for GitHub review

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=level roll=0.0 pitch=0.0 accel_norm=1.00 state=STABLE confidence=96
phase=tilt roll=0.0 pitch=28.6 accel_norm=1.00 state=TRACKING confidence=96
phase=vibration roll=18.9 pitch=22.0 accel_norm=1.19 state=VIBRATION confidence=63
phase=freefall roll=2.8 pitch=2.4 accel_norm=0.09 state=FREEFALL confidence=10
phase=recovery roll=0.6 pitch=4.0 accel_norm=1.00 state=TRACKING confidence=96
```

## Future Extensions

- Full quaternion filter with magnetometer support
- Online gyro bias estimation
- CMSIS-DSP optimized math path
- Motion-triggered interrupt integration
