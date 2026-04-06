#include <stdio.h>

#include "attestation_node.h"

static void hex_encode(const uint8_t *bytes, size_t length, char *out,
                       size_t output_bytes) {
    static const char hex_chars[] = "0123456789abcdef";
    size_t hex_length = length * 2u;

    if (hex_length + 1u > output_bytes) {
        hex_length = output_bytes - 1u;
    }

    for (size_t i = 0; i < hex_length / 2u; ++i) {
        out[i * 2u] = hex_chars[(bytes[i] >> 4u) & 0x0Fu];
        out[i * 2u + 1u] = hex_chars[bytes[i] & 0x0Fu];
    }
    out[hex_length] = '\0';
}

static void print_phase(const char *label, const san_attestation_t *attestation) {
    char token_hex[33];

    if (attestation->status == SAN_STATUS_OK) {
        hex_encode(attestation->token, 16u, token_hex, sizeof(token_hex));
    } else {
        snprintf(token_hex, sizeof(token_hex), "BLOCKED");
    }

    printf("phase=%s boot=%s status=%s counter=%u token=%s\n", label,
           attestation->boot_ok ? "YES" : "NO",
           san_status_name(attestation->status), attestation->counter, token_hex);
}

int main(void) {
    static const uint8_t device_id[SAN_DEVICE_ID_SIZE] = {0x10u, 0x32u, 0x54u, 0x76u,
                                                          0x98u, 0xBAu, 0xDCu, 0xFEu};
    static const uint8_t secret_key[SAN_SECRET_KEY_SIZE] = {
        0x00u, 0x11u, 0x22u, 0x33u, 0x44u, 0x55u, 0x66u, 0x77u,
        0x88u, 0x99u, 0xAAu, 0xBBu, 0xCCu, 0xDDu, 0xEEu, 0xFFu,
        0x10u, 0x21u, 0x32u, 0x43u, 0x54u, 0x65u, 0x76u, 0x87u,
        0x98u, 0xA9u, 0xBAu, 0xCBu, 0xDCu, 0xEDu, 0xFEu, 0x0Fu,
    };
    static const uint8_t golden_firmware[] = "fw-image:v1.0.0:stable";
    static const uint8_t updated_firmware[] = "fw-image:v1.1.0:signed";
    static const uint8_t tampered_firmware[] = "fw-image:v1.1.0:tampered";
    san_device_t device;
    san_challenge_t challenge_a = {{0x01u, 0x03u, 0x05u, 0x07u, 0x09u, 0x0Bu, 0x0Du, 0x0Fu,
                                    0x11u, 0x13u, 0x15u, 0x17u, 0x19u, 0x1Bu, 0x1Du, 0x1Fu},
                                   40u};
    san_challenge_t challenge_b = {{0x21u, 0x23u, 0x25u, 0x27u, 0x29u, 0x2Bu, 0x2Du, 0x2Fu,
                                    0x31u, 0x33u, 0x35u, 0x37u, 0x39u, 0x3Bu, 0x3Du, 0x3Fu},
                                   41u};
    san_attestation_t attestation;

    san_device_init(&device, device_id, secret_key, golden_firmware,
                    sizeof(golden_firmware) - 1u, 40u);

    attestation = san_device_issue_attestation(&device, golden_firmware,
                                               sizeof(golden_firmware) - 1u,
                                               &challenge_a);
    print_phase("golden", &attestation);

    attestation = san_device_issue_attestation(&device, golden_firmware,
                                               sizeof(golden_firmware) - 1u,
                                               &challenge_a);
    print_phase("replay_guard", &attestation);

    san_device_set_expected_firmware(&device, updated_firmware,
                                     sizeof(updated_firmware) - 1u);
    attestation = san_device_issue_attestation(&device, updated_firmware,
                                               sizeof(updated_firmware) - 1u,
                                               &challenge_b);
    print_phase("trusted_update", &attestation);

    attestation = san_device_issue_attestation(&device, tampered_firmware,
                                               sizeof(tampered_firmware) - 1u,
                                               &challenge_b);
    print_phase("tampered", &attestation);

    return 0;
}

