# Smart BMS Firmware

Host-side battery management firmware simulator written in C.

## Features

- Safety-focused state machine
- Fault detection and latching behavior
- Simple state-of-charge estimation
- Cell balancing decision logic
- Unit tests for common scenarios

## Build

```sh
make
make test
make run
```

## Future Extensions

- ADC calibration and hardware abstraction layer
- Contactor and precharge sequencing
- EEPROM-backed fault history
- CAN/UART telemetry export

