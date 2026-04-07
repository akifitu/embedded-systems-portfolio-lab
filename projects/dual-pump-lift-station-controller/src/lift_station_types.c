#include "lift_station_types.h"

const char *lift_state_name(lift_state_t state) {
    switch (state) {
        case LIFT_STATE_IDLE:
            return "IDLE";
        case LIFT_STATE_DRAIN_LEAD:
            return "DRAIN_LEAD";
        case LIFT_STATE_DRAIN_ASSIST:
            return "DRAIN_ASSIST";
        case LIFT_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *lift_command_name(lift_command_t command) {
    switch (command) {
        case LIFT_COMMAND_STOP_ALL:
            return "STOP_ALL";
        case LIFT_COMMAND_RUN_A:
            return "RUN_A";
        case LIFT_COMMAND_RUN_B:
            return "RUN_B";
        case LIFT_COMMAND_RUN_BOTH:
            return "RUN_BOTH";
        default:
            return "UNKNOWN";
    }
}

const char *lift_fault_name(lift_fault_t fault) {
    switch (fault) {
        case LIFT_FAULT_NONE:
            return "NONE";
        case LIFT_FAULT_OVERFLOW:
            return "OVERFLOW";
        case LIFT_FAULT_NO_PUMP_AVAILABLE:
            return "NO_PUMP_AVAILABLE";
        case LIFT_FAULT_PUMP_A_SEAL:
            return "PUMP_A_SEAL";
        case LIFT_FAULT_PUMP_B_SEAL:
            return "PUMP_B_SEAL";
        default:
            return "UNKNOWN";
    }
}

const char *lift_pump_name(lift_pump_t pump) {
    switch (pump) {
        case LIFT_PUMP_A:
            return "A";
        case LIFT_PUMP_B:
            return "B";
        default:
            return "?";
    }
}
