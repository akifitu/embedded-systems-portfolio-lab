# Secure Attestation Node

Host-buildable firmware security model for device attestation.

Docs:
- [Implementation Plan](docs/IMPLEMENTATION_PLAN.md)
- [Architecture Notes](docs/ARCHITECTURE.md)

## Features

- SHA-256 firmware measurement
- HMAC-SHA256 challenge-response attestation
- Monotonic counter based replay protection
- Measurement mismatch blocking for tampered firmware

## Build

```sh
make
make test
make run
```

## Demo Snapshot

```text
phase=golden boot=YES status=OK counter=41 token=f90d4a7232d03ec661a92da43581e5f8
phase=replay_guard boot=YES status=STALE_CHALLENGE counter=41 token=BLOCKED
phase=trusted_update boot=YES status=OK counter=42 token=4fc3bda9798b53048f171e8650cfbc19
phase=tampered boot=NO status=MEASUREMENT_MISMATCH counter=42 token=BLOCKED
```

## Future Extensions

- Real secure-element key backend
- ECDSA or Ed25519 signed manifest verification
- Measured boot chain over multiple firmware regions
- Remote provisioning workflow
