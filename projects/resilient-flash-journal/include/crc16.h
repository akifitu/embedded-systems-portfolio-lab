#ifndef CRC16_H
#define CRC16_H

#include <stddef.h>
#include <stdint.h>

uint16_t rfj_crc16_compute(const uint8_t *data, size_t length);

#endif

