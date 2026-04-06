# Implementation Plan

## Goal

Build a compact embedded attitude estimator that fuses gyro and accelerometer
data to estimate roll and pitch while classifying motion conditions such as
vibration and freefall.

## Step By Step

1. Define an IMU sample format with accelerometer, gyro, and sample period.
2. Implement accelerometer-only tilt references for roll and pitch.
3. Implement a complementary filter that integrates gyro motion and blends back to accelerometer references.
4. Make the blend confidence dynamic so abnormal acceleration reduces trust in the accelerometer.
5. Classify motion state into:
   - `STABLE`
   - `TRACKING`
   - `VIBRATION`
   - `FREEFALL`
6. Create a deterministic demo through level, tilt, vibration, freefall, and recovery phases.
7. Lock behavior with unit tests before pushing.

## Embedded Value

- Shows sensor fusion rather than raw-sensor printing
- Demonstrates handling of bad acceleration conditions during vibration and freefall
- Maps directly to robotics, drones, wearables, and stabilization firmware

