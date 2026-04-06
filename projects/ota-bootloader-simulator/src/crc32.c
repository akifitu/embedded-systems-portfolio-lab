#include "crc32.h"

uint32_t crc32_compute(const uint8_t *data, size_t length) {
    uint32_t crc = 0xFFFFFFFFu;

    for (size_t i = 0; i < length; ++i) {
        crc ^= data[i];
        for (int bit = 0; bit < 8; ++bit) {
            if ((crc & 1u) != 0u) {
                crc = (crc >> 1u) ^ 0xEDB88320u;
            } else {
                crc >>= 1u;
            }
        }
    }

    return ~crc;
}

