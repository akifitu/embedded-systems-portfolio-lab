#ifndef SECURITY_TYPES_H
#define SECURITY_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define SAN_DEVICE_ID_SIZE 8u
#define SAN_SECRET_KEY_SIZE 32u
#define SAN_HASH_SIZE 32u
#define SAN_NONCE_SIZE 16u

typedef enum {
    SAN_STATUS_OK = 0,
    SAN_STATUS_STALE_CHALLENGE,
    SAN_STATUS_MEASUREMENT_MISMATCH
} san_status_t;

typedef struct {
    uint8_t nonce[SAN_NONCE_SIZE];
    uint32_t expected_counter;
} san_challenge_t;

typedef struct {
    san_status_t status;
    bool boot_ok;
    uint32_t counter;
    uint8_t token[SAN_HASH_SIZE];
    uint8_t measurement[SAN_HASH_SIZE];
} san_attestation_t;

const char *san_status_name(san_status_t status);

#endif

