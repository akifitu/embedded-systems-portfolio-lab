# Implementation Plan

1. Define flight states, commands, health labels, and structured inputs/outputs
   so host demos and tests can inspect deterministic safety decisions.
2. Implement a return-margin estimator that converts distance, altitude, and
   wind into a battery budget for return-to-home.
3. Add a geofence evaluator that reports whether the vehicle is inside or
   outside its allowed flight radius.
4. Build a failsafe controller that escalates from mission mode to RTL,
   emergency landing, and final motor cutoff based on link loss, nav loss, and
   critical battery.
5. Add a deterministic host demo and unit tests for arming block, geofence
   breach, link-loss timeout, nav-loss landing, and touchdown disarm.
