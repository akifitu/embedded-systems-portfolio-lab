# Implementation Plan

## Goal

Build a compact Modbus RTU slave firmware model that validates CRC16, reads and
writes holding registers, and returns proper exception frames for invalid
requests.

## Step By Step

1. Define a register map for measurements and writable configuration.
2. Implement Modbus CRC16 using the RTU polynomial and byte order.
3. Implement a register bank with range-checked read and write helpers.
4. Implement RTU request handling for:
   - `0x03` Read Holding Registers
   - `0x06` Write Single Register
5. Implement exception responses for illegal function and illegal address.
6. Create a deterministic demo that replays several requests and prints hex frames.
7. Lock behavior with unit tests before pushing.

## Embedded Value

- Shows industrial protocol literacy beyond generic UART prints
- Demonstrates wire-format parsing and exception semantics
- Maps directly to RS-485 field devices and PLC integration work

