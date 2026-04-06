# OTA Bootloader Simulator

Host-side A/B firmware update simulator inspired by bootloader flows used in
embedded products.

## Features

- Chunked OTA image reception
- CRC32 validation
- Secondary-slot staging
- Test upgrade and automatic rollback
- Permanent upgrade flow

## Build

```sh
make
make test
make run
```

## Future Extensions

- Signature verification with real public keys
- Flash sector erase/write constraints
- Swap metadata emulation
- UART or CAN transport framing

