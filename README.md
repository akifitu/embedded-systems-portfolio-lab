# Embedded Systems Portfolio Lab

This repository is a focused embedded-systems portfolio starter built to be
strong on GitHub even before real hardware is attached. The projects are
written in portable C, compile on the host with a standard C compiler, and
model problems that appear in production firmware teams.

## Projects

### 1. Smart BMS Firmware

Path: `projects/smart-bms-firmware`

Highlights:
- Battery-management state machine
- Fault detection for voltage, temperature, current, and imbalance
- Coulomb-counting based state-of-charge estimator
- Host-side simulation and tests

### 2. OTA Bootloader Simulator

Path: `projects/ota-bootloader-simulator`

Highlights:
- A/B slot update flow
- Staged image reception with CRC32 verification
- Test upgrade, confirm, and rollback behavior
- MCUboot-inspired concepts without external dependencies

### 3. CAN Telemetry Node

Path: `projects/can-telemetry-node`

Highlights:
- Deterministic periodic frame scheduling
- Fault-priority CAN event emission
- SocketCAN-friendly frame formatting
- Host simulation and tests

## Why This Set

These three projects deliberately cover a useful embedded hiring surface:

- Control logic and safety state machines
- Firmware update and boot reliability
- Bus communication and telemetry design

That combination reads much better on GitHub than only toy sensor demos.

## Build

Run everything:

```sh
make all
make test
```

Run demos:

```sh
make run-bms
make run-ota
make run-can
```

Each project is self-contained and can later be split into its own GitHub repo.

## Suggested Next Hardware Targets

- Port the BMS project to STM32 or ESP32 with ADC and GPIO drivers
- Port the OTA simulator to Zephyr or MCUboot integration tests
- Bridge the CAN node to Linux `vcan` or a real MCP2515 interface

## References

- Zephyr native host execution:
  https://docs.zephyrproject.org/3.7.0/boards/native/native_posix/doc/index.html
- SocketCAN overview and `vcan` virtual interfaces:
  https://docs.kernel.org/networking/can.html
- MCUboot image signing and upgrade concepts:
  https://docs.mcuboot.com/signed_images.html
  https://docs.mcuboot.com/imgtool.html
