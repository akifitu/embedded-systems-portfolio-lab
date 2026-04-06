#include "gpsdo_types.h"

const char *gpsdo_state_name(gpsdo_state_t state) {
    switch (state) {
        case GPSDO_STATE_ACQUIRE:
            return "ACQUIRE";
        case GPSDO_STATE_TRACKING:
            return "TRACKING";
        case GPSDO_STATE_HOLDOVER:
            return "HOLDOVER";
        case GPSDO_STATE_FAULT:
            return "FAULT";
    }

    return "UNKNOWN";
}

const char *gpsdo_quality_name(gpsdo_quality_t quality) {
    switch (quality) {
        case GPSDO_QUALITY_SEARCH:
            return "SEARCH";
        case GPSDO_QUALITY_LOCKED:
            return "LOCKED";
        case GPSDO_QUALITY_HOLDOVER:
            return "HOLDOVER";
        case GPSDO_QUALITY_FAULT:
            return "FAULT";
    }

    return "UNKNOWN";
}
