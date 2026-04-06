#ifndef HMAC_SHA256_H
#define HMAC_SHA256_H

#include <stddef.h>
#include <stdint.h>

#include "security_types.h"

void san_hmac_sha256(const uint8_t *key, size_t key_length, const uint8_t *data,
                     size_t data_length, uint8_t digest_out[SAN_HASH_SIZE]);

#endif

