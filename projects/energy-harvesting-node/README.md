# Energy Harvesting Node

Host-buildable firmware model for an energy-aware remote sensing node.

Docs:
- [Implementation Plan](docs/IMPLEMENTATION_PLAN.md)
- [Architecture Notes](docs/ARCHITECTURE.md)

## Features

- Battery and harvested-power aware task selection
- Brownout reserve protection and deep sleep fallback
- Sampling, uplink, and GNSS task gating
- Deterministic wake interval planning

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=dawn mode=BALANCED action=SAMPLE battery=5042mWh backlog=1 reserve=NO wake=15s
phase=sunny mode=PERFORMANCE action=TX battery=5861mWh backlog=0 reserve=NO wake=5s
phase=cloud mode=BALANCED action=SLEEP battery=5691mWh backlog=3 reserve=NO wake=20s
phase=brownout mode=SURVIVAL action=DEEP_SLEEP battery=1816mWh backlog=4 reserve=YES wake=60s
phase=night-recovery mode=BALANCED action=TX battery=4171mWh backlog=3 reserve=NO wake=5s
```

## Future Extensions

- Real PMIC or coulomb-counter backend
- LoRa or LTE uplink cost model
- RTC alarm driven wake scheduling
- Supercapacitor reserve estimation
