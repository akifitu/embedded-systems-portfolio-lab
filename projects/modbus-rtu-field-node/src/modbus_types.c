#include "modbus_types.h"

const char *mbn_exception_name(mbn_exception_t code) {
    switch (code) {
        case MBN_EXCEPTION_ILLEGAL_FUNCTION:
            return "ILLEGAL_FUNCTION";
        case MBN_EXCEPTION_ILLEGAL_ADDRESS:
            return "ILLEGAL_ADDRESS";
        case MBN_EXCEPTION_ILLEGAL_VALUE:
            return "ILLEGAL_VALUE";
        default:
            return "UNKNOWN";
    }
}

