# Resilient Flash Journal

Host-buildable simulator for a power-fail-safe flash event journal.

Docs:
- [Implementation Plan](docs/IMPLEMENTATION_PLAN.md)
- [Architecture Notes](docs/ARCHITECTURE.md)

## Features

- Fixed-slot flash journal with bounded storage
- CRC16 and commit-marker based validity checks
- Recovery scan that ignores torn writes after reboot
- Page erase counting to visualize wear behavior

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
boot_a valid=3 latest_seq=2 latest_type=OVERCURRENT erase=[0 0 0]
boot_b valid=9 latest_seq=11 latest_type=CONFIG_CHANGE erase=[1 0 0]
tail seq=9 type=WATCHDOG_RESET sev=WARNING value=1
tail seq=10 type=POWER_FAIL sev=CRITICAL value=1
tail seq=11 type=CONFIG_CHANGE sev=INFO value=7
```

## Future Extensions

- Real QSPI or SPI NOR backend
- Brownout interrupt driven flush path
- Binary search over journal generations
- Compression for repeated fault records
