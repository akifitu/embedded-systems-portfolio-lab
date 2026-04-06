#include "crc16.h"

uint16_t rfj_crc16_compute(const uint8_t *data, size_t length) {
    uint16_t crc = 0xFFFFu;

    for (size_t i = 0; i < length; ++i) {
        crc ^= (uint16_t)(data[i] << 8u);
        for (int bit = 0; bit < 8; ++bit) {
            if ((crc & 0x8000u) != 0u) {
                crc = (uint16_t)((crc << 1u) ^ 0x1021u);
            } else {
                crc <<= 1u;
            }
        }
    }

    return crc;
}

