#ifndef REGISTER_BANK_H
#define REGISTER_BANK_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "modbus_types.h"

typedef struct {
    uint16_t holding[MBN_REGISTER_COUNT];
} mbn_register_bank_t;

void mbn_register_bank_init(mbn_register_bank_t *bank);
bool mbn_register_bank_read(const mbn_register_bank_t *bank, uint16_t address,
                            uint16_t count, uint16_t *values_out);
bool mbn_register_bank_write(mbn_register_bank_t *bank, uint16_t address,
                             uint16_t value);

#endif

