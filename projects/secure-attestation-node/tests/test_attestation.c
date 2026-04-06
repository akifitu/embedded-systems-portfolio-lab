#include <stdio.h>
#include <string.h>

#include "attestation_node.h"
#include "hmac_sha256.h"
#include "sha256.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static void hex_encode(const uint8_t *bytes, size_t length, char *out) {
    static const char hex_chars[] = "0123456789abcdef";

    for (size_t i = 0; i < length; ++i) {
        out[i * 2u] = hex_chars[(bytes[i] >> 4u) & 0x0Fu];
        out[i * 2u + 1u] = hex_chars[bytes[i] & 0x0Fu];
    }
    out[length * 2u] = '\0';
}

static int test_sha256_abc(void) {
    uint8_t digest[SAN_HASH_SIZE];
    char hex[65];

    san_sha256_compute((const uint8_t *)"abc", 3u, digest);
    hex_encode(digest, SAN_HASH_SIZE, hex);
    ASSERT_TRUE(strcmp(hex,
                       "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad") == 0);
    return 0;
}

static int test_hmac_reference(void) {
    uint8_t digest[SAN_HASH_SIZE];
    char hex[65];

    san_hmac_sha256((const uint8_t *)"key", 3u,
                    (const uint8_t *)"The quick brown fox jumps over the lazy dog",
                    43u, digest);
    hex_encode(digest, SAN_HASH_SIZE, hex);
    ASSERT_TRUE(strcmp(hex,
                       "f7bc83f430538424b13298e6aa6fb143ef4d59a14946175997479dbc2d1a3cd8") == 0);
    return 0;
}

static int test_attestation_counter_and_replay_guard(void) {
    static const uint8_t device_id[SAN_DEVICE_ID_SIZE] = {0x10u, 0x32u, 0x54u, 0x76u,
                                                          0x98u, 0xBAu, 0xDCu, 0xFEu};
    static const uint8_t secret_key[SAN_SECRET_KEY_SIZE] = {
        0x00u, 0x11u, 0x22u, 0x33u, 0x44u, 0x55u, 0x66u, 0x77u,
        0x88u, 0x99u, 0xAAu, 0xBBu, 0xCCu, 0xDDu, 0xEEu, 0xFFu,
        0x10u, 0x21u, 0x32u, 0x43u, 0x54u, 0x65u, 0x76u, 0x87u,
        0x98u, 0xA9u, 0xBAu, 0xCBu, 0xDCu, 0xEDu, 0xFEu, 0x0Fu,
    };
    static const uint8_t firmware[] = "fw-image:v1.0.0:stable";
    san_device_t device;
    san_challenge_t challenge = {{0}, 40u};
    san_attestation_t attestation;

    san_device_init(&device, device_id, secret_key, firmware, sizeof(firmware) - 1u,
                    40u);
    attestation = san_device_issue_attestation(&device, firmware, sizeof(firmware) - 1u,
                                               &challenge);
    ASSERT_TRUE(attestation.status == SAN_STATUS_OK);
    ASSERT_TRUE(attestation.counter == 41u);

    attestation = san_device_issue_attestation(&device, firmware, sizeof(firmware) - 1u,
                                               &challenge);
    ASSERT_TRUE(attestation.status == SAN_STATUS_STALE_CHALLENGE);
    ASSERT_TRUE(attestation.counter == 41u);
    return 0;
}

static int test_measurement_mismatch_blocks_token(void) {
    static const uint8_t device_id[SAN_DEVICE_ID_SIZE] = {1u, 2u, 3u, 4u, 5u, 6u, 7u, 8u};
    static const uint8_t secret_key[SAN_SECRET_KEY_SIZE] = {0};
    static const uint8_t golden[] = "golden";
    static const uint8_t tampered[] = "tampered";
    san_device_t device;
    san_challenge_t challenge = {{0}, 5u};
    san_attestation_t attestation;

    san_device_init(&device, device_id, secret_key, golden, sizeof(golden) - 1u, 5u);
    attestation = san_device_issue_attestation(&device, tampered, sizeof(tampered) - 1u,
                                               &challenge);
    ASSERT_TRUE(attestation.status == SAN_STATUS_MEASUREMENT_MISMATCH);
    ASSERT_TRUE(!attestation.boot_ok);
    ASSERT_TRUE(device.counter == 5u);
    return 0;
}

static int test_trusted_update_changes_expected_measurement(void) {
    static const uint8_t device_id[SAN_DEVICE_ID_SIZE] = {1u, 1u, 1u, 1u, 1u, 1u, 1u, 1u};
    static const uint8_t secret_key[SAN_SECRET_KEY_SIZE] = {2u};
    static const uint8_t firmware_a[] = "firmware-a";
    static const uint8_t firmware_b[] = "firmware-b";
    san_device_t device;
    san_challenge_t challenge = {{0}, 10u};
    san_attestation_t attestation;

    san_device_init(&device, device_id, secret_key, firmware_a, sizeof(firmware_a) - 1u,
                    10u);
    san_device_set_expected_firmware(&device, firmware_b, sizeof(firmware_b) - 1u);
    attestation = san_device_issue_attestation(&device, firmware_b, sizeof(firmware_b) - 1u,
                                               &challenge);
    ASSERT_TRUE(attestation.status == SAN_STATUS_OK);
    ASSERT_TRUE(attestation.counter == 11u);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_sha256_abc() == 0);
    ASSERT_TRUE(test_hmac_reference() == 0);
    ASSERT_TRUE(test_attestation_counter_and_replay_guard() == 0);
    ASSERT_TRUE(test_measurement_mismatch_blocks_token() == 0);
    ASSERT_TRUE(test_trusted_update_changes_expected_measurement() == 0);
    printf("secure-attestation-node tests passed\n");
    return 0;
}
