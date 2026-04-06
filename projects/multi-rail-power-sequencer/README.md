# Multi-Rail Power Sequencer

Portable C model of a board power-sequencing controller. The project enables
dependent rails in order, waits for power-good confirmation, retries failed
bring-up attempts, and latches brownout or short-circuit faults while forcing
all rails off.

## Why It Matters

- Shows real board-management logic used around SoCs, FPGAs, and radio modules
- Demonstrates power-good timing, retry strategy, and safe shutdown behavior
- Maps naturally to a small supervisor MCU or PMIC companion firmware

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=boot_start state=STARTUP rail=CORE_1V0 mask=0x1 retries=0 progress=0 faults=none
phase=io_wait state=STARTUP rail=IO_1V8 mask=0x3 retries=0 progress=25 faults=none
phase=stable state=STABLE rail=NONE mask=0xF retries=0 progress=100 faults=none
phase=shutdown_complete state=IDLE rail=NONE mask=0x0 retries=0 progress=0 faults=none
phase=pg_timeout_retry state=RETRY_WAIT rail=CORE_1V0 mask=0x0 retries=1 progress=0 faults=pg_timeout
phase=recovered state=STABLE rail=NONE mask=0xF retries=1 progress=100 faults=none
phase=brownout_fault state=FAULT rail=NONE mask=0x0 retries=1 progress=100 faults=brownout
```

## Implementation Notes

- `rail_plan.c` defines the ordered rail list and PG timeout budgets
- `power_sequencer.c` implements startup, retry, stable, shutdown, and fault states
- `main.c` drives nominal bring-up, retry recovery, brownout fault, and clean shutdown

## Hardware Path

Natural next step: STM32 or RP2040 supervisor that drives PMIC enable pins,
reads power-good GPIOs, monitors brownout through ADC, and logs rail faults to
nonvolatile storage.
