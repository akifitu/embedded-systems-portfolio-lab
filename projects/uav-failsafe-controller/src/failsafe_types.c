#include "failsafe_types.h"

const char *uav_state_name(uav_state_t state) {
    switch (state) {
        case UAV_STATE_STANDBY:
            return "STANDBY";
        case UAV_STATE_MISSION:
            return "MISSION";
        case UAV_STATE_RTL:
            return "RTL";
        case UAV_STATE_LAND:
            return "LAND";
        case UAV_STATE_DISARMED:
            return "DISARMED";
        default:
            return "UNKNOWN";
    }
}

const char *uav_command_name(uav_command_t command) {
    switch (command) {
        case UAV_COMMAND_HOLD:
            return "HOLD";
        case UAV_COMMAND_PROCEED:
            return "PROCEED";
        case UAV_COMMAND_RETURN_HOME:
            return "RETURN_HOME";
        case UAV_COMMAND_DESCEND:
            return "DESCEND";
        case UAV_COMMAND_CUT_MOTORS:
            return "CUT_MOTORS";
        default:
            return "UNKNOWN";
    }
}

const char *uav_reason_name(uav_reason_t reason) {
    switch (reason) {
        case UAV_REASON_NONE:
            return "NONE";
        case UAV_REASON_ARMING_BLOCK:
            return "ARMING_BLOCK";
        case UAV_REASON_GEOFENCE:
            return "GEOFENCE";
        case UAV_REASON_LINK_LOSS:
            return "LINK_LOSS";
        case UAV_REASON_RETURN_MARGIN:
            return "RETURN_MARGIN";
        case UAV_REASON_NAV_LOSS:
            return "NAV_LOSS";
        case UAV_REASON_CRITICAL_BATTERY:
            return "CRITICAL_BATTERY";
        default:
            return "UNKNOWN";
    }
}

const char *uav_health_name(uav_health_t health) {
    switch (health) {
        case UAV_HEALTH_GREEN:
            return "GREEN";
        case UAV_HEALTH_YELLOW:
            return "YELLOW";
        case UAV_HEALTH_RED:
            return "RED";
        default:
            return "UNKNOWN";
    }
}
