# Implementation Plan

1. Define a deterministic battery-swap dock state machine with validation,
   precharge, docked, release, and fault behavior.
2. Model the voltage-delta checks that gate precharge completion and mismatch
   handling before the main path closes.
3. Separate output policy from the controller so latch, precharge, fan, and
   main contactor behavior stay easy to reason about.
4. Add host-buildable tests for nominal docking, release, auth failure,
   thermal lockout, precharge timeout, and safe reset.
5. Integrate the project into the portfolio root Makefile, README, and roadmap
   so it builds with the rest of the collection.
