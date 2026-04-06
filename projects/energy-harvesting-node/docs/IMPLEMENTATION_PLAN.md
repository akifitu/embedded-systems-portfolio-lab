# Implementation Plan

## Goal

Build a small embedded power manager for a remote sensor node that chooses when
to sample, transmit, acquire GNSS, or sleep based on harvested power, battery
state, backlog, and brownout reserve.

## Step By Step

1. Define a compact node status input carrying battery energy, harvested power, telemetry backlog, and deadlines.
2. Create a budget model that updates battery energy after task execution.
3. Implement task-selection rules for:
   - `SAMPLE`
   - `TX`
   - `GNSS`
   - `SLEEP`
   - `DEEP_SLEEP`
4. Add operating modes:
   - `PERFORMANCE`
   - `BALANCED`
   - `SURVIVAL`
5. Compute the next wake interval from current mode and urgency.
6. Drive a deterministic demo through dawn, sunny, cloud, brownout, and recovery phases.
7. Lock the behavior with unit tests before pushing.

## Embedded Value

- Shows low-power thinking instead of always-on firmware
- Demonstrates resource arbitration under hard energy limits
- Maps directly to solar nodes, wildlife trackers, and industrial telemetry devices

