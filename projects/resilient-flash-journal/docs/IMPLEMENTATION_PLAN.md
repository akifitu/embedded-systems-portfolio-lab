# Implementation Plan

## Goal

Build a small firmware-oriented flash persistence layer that survives power
loss, reboots cleanly, and makes wear behavior visible in a deterministic host
simulation.

## Step By Step

1. Define a fixed journal slot layout with sequence number, event payload, CRC16, and commit marker.
2. Model flash pages and erase cycles in a bounded raw-memory simulator.
3. Implement append logic that writes records sequentially and erases pages on reuse.
4. Implement mount-time recovery that scans all slots, validates records, and ignores torn writes.
5. Add replay helpers to inspect the latest valid records in sequence order.
6. Create a deterministic demo with:
   - normal writes
   - a torn write
   - reboot recovery
   - page rotation and erase counting
7. Lock behavior with unit tests before pushing to GitHub.

## Embedded Value

- Shows how firmware handles persistence under power loss
- Demonstrates wear-awareness instead of naive append-only storage
- Mirrors the constraints of NOR/QSPI flash in a minimal, interview-friendly form

