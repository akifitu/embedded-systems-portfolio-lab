#ifndef MODBUS_CRC_H
#define MODBUS_CRC_H

#include <stddef.h>
#include <stdint.h>

uint16_t mbn_crc16_compute(const uint8_t *data, size_t length);

#endif

