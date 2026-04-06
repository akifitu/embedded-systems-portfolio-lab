#include "attestation_node.h"

#include <string.h>

#include "hmac_sha256.h"
#include "sha256.h"

static void write_u32_be(uint8_t *buffer, size_t offset, uint32_t value) {
    buffer[offset] = (uint8_t)((value >> 24u) & 0xFFu);
    buffer[offset + 1u] = (uint8_t)((value >> 16u) & 0xFFu);
    buffer[offset + 2u] = (uint8_t)((value >> 8u) & 0xFFu);
    buffer[offset + 3u] = (uint8_t)(value & 0xFFu);
}

void san_device_set_expected_firmware(san_device_t *device,
                                      const uint8_t *firmware_image,
                                      size_t firmware_length) {
    san_sha256_compute(firmware_image, firmware_length, device->expected_measurement);
}

void san_device_init(san_device_t *device,
                     const uint8_t device_id[SAN_DEVICE_ID_SIZE],
                     const uint8_t secret_key[SAN_SECRET_KEY_SIZE],
                     const uint8_t *firmware_image, size_t firmware_length,
                     uint32_t starting_counter) {
    memcpy(device->device_id, device_id, SAN_DEVICE_ID_SIZE);
    memcpy(device->secret_key, secret_key, SAN_SECRET_KEY_SIZE);
    device->counter = starting_counter;
    san_device_set_expected_firmware(device, firmware_image, firmware_length);
}

bool san_device_verify_boot(const san_device_t *device,
                            const uint8_t *firmware_image,
                            size_t firmware_length,
                            uint8_t measurement_out[SAN_HASH_SIZE]) {
    san_sha256_compute(firmware_image, firmware_length, measurement_out);
    return memcmp(measurement_out, device->expected_measurement, SAN_HASH_SIZE) == 0;
}

san_attestation_t san_device_issue_attestation(
    san_device_t *device, const uint8_t *firmware_image, size_t firmware_length,
    const san_challenge_t *challenge) {
    san_attestation_t attestation = {0};
    uint8_t payload[SAN_DEVICE_ID_SIZE + SAN_NONCE_SIZE + SAN_HASH_SIZE + 4u];

    attestation.boot_ok = san_device_verify_boot(device, firmware_image,
                                                 firmware_length,
                                                 attestation.measurement);
    attestation.counter = device->counter;

    if (!attestation.boot_ok) {
        attestation.status = SAN_STATUS_MEASUREMENT_MISMATCH;
        return attestation;
    }

    if (challenge->expected_counter != device->counter) {
        attestation.status = SAN_STATUS_STALE_CHALLENGE;
        return attestation;
    }

    device->counter++;
    attestation.counter = device->counter;
    attestation.status = SAN_STATUS_OK;

    memcpy(payload, device->device_id, SAN_DEVICE_ID_SIZE);
    memcpy(&payload[SAN_DEVICE_ID_SIZE], challenge->nonce, SAN_NONCE_SIZE);
    memcpy(&payload[SAN_DEVICE_ID_SIZE + SAN_NONCE_SIZE], attestation.measurement,
           SAN_HASH_SIZE);
    write_u32_be(payload, SAN_DEVICE_ID_SIZE + SAN_NONCE_SIZE + SAN_HASH_SIZE,
                 attestation.counter);

    san_hmac_sha256(device->secret_key, SAN_SECRET_KEY_SIZE, payload,
                    sizeof(payload), attestation.token);
    return attestation;
}

