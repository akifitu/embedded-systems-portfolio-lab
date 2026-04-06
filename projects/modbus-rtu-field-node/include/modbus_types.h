#ifndef MODBUS_TYPES_H
#define MODBUS_TYPES_H

#include <stddef.h>
#include <stdint.h>

#define MBN_MAX_FRAME 64u
#define MBN_REGISTER_COUNT 32u
#define MBN_SLAVE_ID 0x11u

typedef enum {
    MBN_EXCEPTION_ILLEGAL_FUNCTION = 1,
    MBN_EXCEPTION_ILLEGAL_ADDRESS = 2,
    MBN_EXCEPTION_ILLEGAL_VALUE = 3
} mbn_exception_t;

const char *mbn_exception_name(mbn_exception_t code);

#endif

