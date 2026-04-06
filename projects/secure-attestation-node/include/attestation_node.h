#ifndef ATTESTATION_NODE_H
#define ATTESTATION_NODE_H

#include <stddef.h>
#include <stdint.h>

#include "security_types.h"

typedef struct {
    uint8_t device_id[SAN_DEVICE_ID_SIZE];
    uint8_t secret_key[SAN_SECRET_KEY_SIZE];
    uint8_t expected_measurement[SAN_HASH_SIZE];
    uint32_t counter;
} san_device_t;

void san_device_init(san_device_t *device,
                     const uint8_t device_id[SAN_DEVICE_ID_SIZE],
                     const uint8_t secret_key[SAN_SECRET_KEY_SIZE],
                     const uint8_t *firmware_image, size_t firmware_length,
                     uint32_t starting_counter);
void san_device_set_expected_firmware(san_device_t *device,
                                      const uint8_t *firmware_image,
                                      size_t firmware_length);
bool san_device_verify_boot(const san_device_t *device,
                            const uint8_t *firmware_image,
                            size_t firmware_length,
                            uint8_t measurement_out[SAN_HASH_SIZE]);
san_attestation_t san_device_issue_attestation(
    san_device_t *device, const uint8_t *firmware_image, size_t firmware_length,
    const san_challenge_t *challenge);

#endif

