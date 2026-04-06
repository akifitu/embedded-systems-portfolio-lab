# Implementation Plan

## Goal

Build a compact embedded security module that measures firmware, compares it
with a trusted expected hash, and only issues challenge-response attestations
when both the firmware measurement and monotonic counter are valid.

## Step By Step

1. Implement SHA-256 for firmware measurement.
2. Implement HMAC-SHA256 for challenge-response token generation.
3. Define a device state with:
   - device identifier
   - secret key
   - expected firmware hash
   - monotonic attestation counter
4. Add boot-time firmware measurement verification.
5. Add attestation issuance rules:
   - reject stale challenge counters
   - reject measurement mismatches
   - increment counter only on successful attestation
6. Create a deterministic demo with:
   - golden firmware
   - stale challenge replay
   - trusted firmware update
   - tampered firmware
7. Lock behavior with unit tests before pushing.

## Embedded Value

- Shows security thinking beyond transport checksums
- Demonstrates measured-boot style validation and replay prevention
- Maps directly to secure elements, TrustZone, or TPM-like flows

