# Dual-Pump Lift Station Controller

Portable C model of a wastewater lift-station controller. The project alternates
lead/lag pumps between cycles, starts an assist pump at high-high level,
detects pump seal faults and unavailable pumps, and supports a reset-based
recovery path after lockout.

## Why It Matters

- Shows industrial fluid-control logic rather than only protocol or sensor demos
- Demonstrates embedded sequencing for lead-lag alternation, assist staging, and fault lockout
- Maps directly to municipal pumping, dewatering skids, and PLC-adjacent embedded roles

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=idle state=IDLE cmd=STOP_ALL lead=A level=28 inflow=40 ready=AB fault=NONE
phase=start_a state=DRAIN_LEAD cmd=RUN_A lead=A level=72 inflow=120 ready=AB fault=NONE
phase=assist_b state=DRAIN_ASSIST cmd=RUN_BOTH lead=A level=91 inflow=180 ready=AB fault=NONE
phase=cycle_swap state=IDLE cmd=STOP_ALL lead=B level=30 inflow=35 ready=AB fault=NONE
phase=single_backup state=DRAIN_LEAD cmd=RUN_B lead=B level=70 inflow=110 ready=B fault=NONE
phase=seal_fault state=FAULT cmd=STOP_ALL lead=B level=68 inflow=100 ready=AB fault=PUMP_B_SEAL
phase=recovered state=IDLE cmd=STOP_ALL lead=B level=24 inflow=20 ready=AB fault=NONE
```

## Implementation Notes

- `level_policy.c` defines start, stop, assist, and overflow thresholds
- `pump_arbiter.c` selects pump A/B or both based on alternation and readiness
- `lift_station_controller.c` handles idle, lead drain, assist drain, fault, and reset flow

## Hardware Path

Natural next step: port the controller to an STM32 or PLC-class MCU with
ultrasonic or pressure level sensing, pump contactor outputs, seal-fail inputs,
and a local HMI alarm stack.
