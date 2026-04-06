# Architecture

## Overview

The project models a small supervisor firmware loop that manages multiple board
power rails. Each control step:

1. Decides whether startup, retry, stable supervision, or shutdown is active
2. Enables the next rail in the required dependency order
3. Waits for the corresponding power-good signal inside a bounded timeout
4. Retries or faults if a rail does not come up correctly
5. Forces all rails off on brownout or short-circuit events

## Modules

- `sequencer_types.*`: shared enums, fault flags, and formatting helpers
- `rail_plan.*`: rail names, order, and PG timeout budgets
- `power_sequencer.*`: main state machine and rail mask management
- `main.c`: deterministic bring-up and fault scenarios

## State Model

- `IDLE`: all rails off
- `STARTUP`: rails are enabled one by one while PG is checked
- `STABLE`: all rails are up and supervised
- `RETRY_WAIT`: bring-up paused after a PG timeout
- `SHUTDOWN`: rails are turned off in reverse order
- `FAULT`: rails off until software explicitly clears the fault

## Fault Model

- `PG_TIMEOUT`: a rail never asserted power-good
- `BROWNOUT`: system input sagged below the safe threshold
- `SHORT_CIRCUIT`: rail fault input requested immediate shutdown
- `RETRY_EXHAUSTED`: too many startup retries were attempted
