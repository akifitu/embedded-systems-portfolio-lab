# Implementation Plan

1. Define controller states, pump commands, fault labels, and shared
   input/output structures so host demos and tests can inspect deterministic behavior.
2. Implement a level-threshold policy for start, stop, high-high assist, and
   overflow conditions.
3. Add a pump arbiter that alternates lead/lag assignment and falls back to the
   healthy pump when one unit is unavailable.
4. Build a lift-station controller that sequences single-pump drain, assist
   drain, seal-fault lockout, and reset-based recovery.
5. Add a deterministic demo and unit tests for idle, lead start, assist mode,
   alternation swap, backup-only pumping, fault trip, and recovery.
