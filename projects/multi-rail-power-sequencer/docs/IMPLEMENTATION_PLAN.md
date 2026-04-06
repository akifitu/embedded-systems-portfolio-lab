# Implementation Plan

1. Define sequencer states, rail identifiers, fault flags, and text helpers so
   demos and tests can inspect deterministic controller output.
2. Implement an ordered rail plan with per-rail power-good timeout budgets.
3. Build a top-level sequencer that handles startup, retry cooldown, stable
   supervision, reverse-order shutdown, and sticky fault handling.
4. Add a deterministic host demo covering nominal boot, PG-timeout retry,
   brownout fault, and shutdown.
5. Add unit tests for successful bring-up, retry recovery, retry exhaustion,
   brownout shutdown, and commanded power-down.
