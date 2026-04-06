#include "security_types.h"

const char *san_status_name(san_status_t status) {
    switch (status) {
        case SAN_STATUS_OK:
            return "OK";
        case SAN_STATUS_STALE_CHALLENGE:
            return "STALE_CHALLENGE";
        case SAN_STATUS_MEASUREMENT_MISMATCH:
            return "MEASUREMENT_MISMATCH";
        default:
            return "UNKNOWN";
    }
}

