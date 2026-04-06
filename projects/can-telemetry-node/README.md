# CAN Telemetry Node

Portable C simulation of a CAN-speaking telemetry node.

## Features

- Periodic status and thermal frames
- Immediate fault event reporting
- CAN ID based priority ordering
- SocketCAN-style frame formatting

## Build

```sh
make
make test
make run
```

## Future Extensions

- Real SocketCAN `vcan` sender bridge
- DBC file generation
- ISO-TP transport experiment
- Fault debounce and diagnostic trouble codes

