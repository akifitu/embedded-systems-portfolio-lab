#include "imu_types.h"

const char *iae_motion_state_name(iae_motion_state_t state) {
    switch (state) {
        case IAE_STATE_STABLE:
            return "STABLE";
        case IAE_STATE_TRACKING:
            return "TRACKING";
        case IAE_STATE_VIBRATION:
            return "VIBRATION";
        case IAE_STATE_FREEFALL:
            return "FREEFALL";
        default:
            return "UNKNOWN";
    }
}

