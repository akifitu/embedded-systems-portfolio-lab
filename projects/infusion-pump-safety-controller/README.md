# Infusion Pump Safety Controller

Portable C model of a volumetric infusion pump controller. The project handles
line priming, infusion delivery, paused therapy, keep-vein-open fallback, and
safety shutdown on occlusion, air-in-line, door-open, or empty-reservoir
conditions.

## Why It Matters

- Shows medical-device style safety logic instead of only generic control loops
- Demonstrates occlusion/air detection, latch-and-alarm behavior, and KVO fallback
- Maps directly to infusion pump, motor-control, and supervised-therapy firmware roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=idle state=IDLE cmd=HOLD_PUMP fault=NONE motor=OFF clamp=CLOSED buzzer=OFF rate=0mLph delivered=0.0mL remaining=24.0mL reservoir=120.0mL pressure=18kPa
phase=priming state=PRIMING cmd=PRIME_LINE fault=NONE motor=ON clamp=OPEN buzzer=OFF rate=300mLph delivered=0.0mL remaining=24.0mL reservoir=118.5mL pressure=24kPa
phase=infusing state=INFUSING cmd=RUN_INFUSION fault=NONE motor=ON clamp=OPEN buzzer=OFF rate=120mLph delivered=12.0mL remaining=12.0mL reservoir=106.5mL pressure=42kPa
phase=paused state=PAUSED cmd=PAUSE_INFUSION fault=NONE motor=OFF clamp=CLOSED buzzer=OFF rate=0mLph delivered=12.0mL remaining=12.0mL reservoir=106.5mL pressure=28kPa
phase=kvo state=KVO cmd=KEEP_VEIN_OPEN fault=NONE motor=ON clamp=OPEN buzzer=OFF rate=5mLph delivered=24.0mL remaining=0.0mL reservoir=94.5mL pressure=38kPa
phase=occlusion_alarm state=ALARM cmd=STOP_AND_ALARM fault=OCCLUSION motor=OFF clamp=CLOSED buzzer=ON rate=0mLph delivered=12.0mL remaining=12.0mL reservoir=106.5mL pressure=145kPa
phase=reset_ready state=IDLE cmd=RESET_PUMP fault=NONE motor=OFF clamp=CLOSED buzzer=OFF rate=0mLph delivered=0.0mL remaining=24.0mL reservoir=106.5mL pressure=20kPa
```

## Implementation Notes

- `safety_monitor.c` prioritizes air-in-line, occlusion, door-open, and empty-reservoir shutdown causes
- `dose_engine.c` tracks deterministic delivery progress and the transition into KVO mode
- `infusion_pump_controller.c` implements priming, infusion, pause/resume, latched alarm, and reset behavior

## Hardware Path

Natural next step: port the controller to STM32, MSP430, or an NXP medical
MCU with stepper or DC motor drive, pressure sensor ADC, air-in-line detector,
door interlock input, buzzer, and clamp actuator feedback.
