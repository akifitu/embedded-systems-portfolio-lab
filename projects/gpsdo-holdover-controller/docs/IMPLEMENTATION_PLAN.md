# Implementation Plan

1. Define controller states, quality labels, fault flags, and input/output
   structures so demos and tests can inspect deterministic timing behavior.
2. Implement a drift model that converts temperature change into holdover trim
   bias and uncertainty growth.
3. Build a controller that acquires PPS lock, disciplines a trim DAC during
   tracking, enters holdover when PPS disappears, and faults on implausible PPS.
4. Add a deterministic host demo for warmup, lock, holdover, relock, and bad-PPS fault.
5. Add unit tests for lock acquisition, holdover uncertainty growth, relock
   reset, and bad-PPS rejection.
