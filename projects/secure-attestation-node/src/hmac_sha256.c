#include "hmac_sha256.h"

#include <string.h>

#include "sha256.h"

void san_hmac_sha256(const uint8_t *key, size_t key_length, const uint8_t *data,
                     size_t data_length, uint8_t digest_out[SAN_HASH_SIZE]) {
    uint8_t key_block[64];
    uint8_t inner_hash[SAN_HASH_SIZE];
    uint8_t inner_buffer[64 + 512];
    uint8_t outer_buffer[64 + SAN_HASH_SIZE];

    memset(key_block, 0, sizeof(key_block));
    if (key_length > 64u) {
        san_sha256_compute(key, key_length, key_block);
    } else {
        memcpy(key_block, key, key_length);
    }

    for (size_t i = 0; i < 64u; ++i) {
        inner_buffer[i] = (uint8_t)(key_block[i] ^ 0x36u);
        outer_buffer[i] = (uint8_t)(key_block[i] ^ 0x5Cu);
    }

    memcpy(&inner_buffer[64], data, data_length);
    san_sha256_compute(inner_buffer, 64u + data_length, inner_hash);
    memcpy(&outer_buffer[64], inner_hash, SAN_HASH_SIZE);
    san_sha256_compute(outer_buffer, 64u + SAN_HASH_SIZE, digest_out);
}

