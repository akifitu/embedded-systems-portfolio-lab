# Modbus RTU Field Node

Host-buildable simulator for an industrial Modbus RTU slave device.

Docs:
- [Implementation Plan](docs/IMPLEMENTATION_PLAN.md)
- [Architecture Notes](docs/ARCHITECTURE.md)

## Features

- Modbus CRC16 frame validation
- Holding register map for live measurements and configuration
- Function code support for `0x03` and `0x06`
- Illegal-function and illegal-address exception responses

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
req1 11 03 00 00 00 04 46 99
rsp1 11 03 08 09 29 00 18 00 3C 00 02 D8 70
req2 11 06 00 11 00 19 1A 95
rsp2 11 06 00 11 00 19 1A 95
req3 11 03 00 10 00 02 C7 5E
rsp3 11 03 04 00 41 00 19 7A 2C
rsp4 11 93 01 8C F5
```

## Future Extensions

- UART byte-stream parser with silent interval timing
- RS-485 direction control hooks
- More function codes such as `0x10`
- Coil and input-register separation
