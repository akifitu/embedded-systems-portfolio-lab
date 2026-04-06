#ifndef MODBUS_RTU_H
#define MODBUS_RTU_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "register_bank.h"

typedef struct {
    uint8_t slave_id;
    mbn_register_bank_t registers;
} mbn_device_t;

void mbn_device_init(mbn_device_t *device, uint8_t slave_id);
bool mbn_device_process_frame(mbn_device_t *device, const uint8_t *request,
                              size_t request_length, uint8_t *response_out,
                              size_t *response_length_out);

#endif

