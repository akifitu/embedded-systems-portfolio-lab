#include "register_bank.h"

void mbn_register_bank_init(mbn_register_bank_t *bank) {
    for (size_t i = 0; i < MBN_REGISTER_COUNT; ++i) {
        bank->holding[i] = 0u;
    }

    bank->holding[0x00] = 2345u;
    bank->holding[0x01] = 24u;
    bank->holding[0x02] = 60u;
    bank->holding[0x03] = 2u;
    bank->holding[0x10] = 65u;
    bank->holding[0x11] = 20u;
}

bool mbn_register_bank_read(const mbn_register_bank_t *bank, uint16_t address,
                            uint16_t count, uint16_t *values_out) {
    if (count == 0u || (address + count) > MBN_REGISTER_COUNT) {
        return false;
    }

    for (uint16_t i = 0u; i < count; ++i) {
        values_out[i] = bank->holding[address + i];
    }
    return true;
}

bool mbn_register_bank_write(mbn_register_bank_t *bank, uint16_t address,
                             uint16_t value) {
    if (address >= MBN_REGISTER_COUNT) {
        return false;
    }

    if (address < 0x10u) {
        return false;
    }

    bank->holding[address] = value;
    return true;
}

