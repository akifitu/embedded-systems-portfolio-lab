# Implementation Plan

1. Define pump states, commands, fault labels, and shared I/O structures so
   demos and tests can inspect deterministic therapy behavior.
2. Implement a safety monitor that prioritizes air-in-line, occlusion,
   door-open, and empty-reservoir conditions.
3. Add a dose engine that advances programmed therapy volume and signals when
   the controller should fall back to KVO mode.
4. Build a controller that handles idle, priming, infusion, pause/resume, KVO,
   latched alarms, and safe reset.
5. Add a deterministic demo and unit tests for priming, infusion progress,
   pause/resume, KVO transition, fault shutdown, and alarm recovery.
