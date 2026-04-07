# Implementation Plan

1. Define EVSE states, pilot-state labels, commands, faults, and shared
   input/output structures for deterministic host demos and tests.
2. Implement a pilot decoder that maps control-pilot voltage to state A/B/C/F
   and converts PWM duty into advertised current.
3. Add a current policy that applies cable-rating limits and thermal derating
   before a hard overtemperature trip.
4. Build an EVSE controller that sequences contactor close, charge enable,
   GFCI fault handling, contactor-fault detection, and cooldown recovery.
5. Add a deterministic demo and unit tests for unplugged idle, connected
   advertisement, charging, thermal derating, GFCI trip, and recovery.
