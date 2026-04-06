#include "node_types.h"

const char *ehn_action_name(ehn_action_t action) {
    switch (action) {
        case EHN_ACTION_SLEEP:
            return "SLEEP";
        case EHN_ACTION_SAMPLE:
            return "SAMPLE";
        case EHN_ACTION_TX:
            return "TX";
        case EHN_ACTION_GNSS:
            return "GNSS";
        case EHN_ACTION_DEEP_SLEEP:
            return "DEEP_SLEEP";
        default:
            return "UNKNOWN";
    }
}

const char *ehn_mode_name(ehn_mode_t mode) {
    switch (mode) {
        case EHN_MODE_PERFORMANCE:
            return "PERFORMANCE";
        case EHN_MODE_BALANCED:
            return "BALANCED";
        case EHN_MODE_SURVIVAL:
            return "SURVIVAL";
        default:
            return "UNKNOWN";
    }
}

