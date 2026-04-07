#include "pump_types.h"

const char *pump_state_name(pump_state_t state) {
    switch (state) {
        case PUMP_STATE_IDLE:
            return "IDLE";
        case PUMP_STATE_PRIMING:
            return "PRIMING";
        case PUMP_STATE_INFUSING:
            return "INFUSING";
        case PUMP_STATE_PAUSED:
            return "PAUSED";
        case PUMP_STATE_KVO:
            return "KVO";
        case PUMP_STATE_ALARM:
            return "ALARM";
        default:
            return "UNKNOWN";
    }
}

const char *pump_command_name(pump_command_t command) {
    switch (command) {
        case PUMP_COMMAND_HOLD_PUMP:
            return "HOLD_PUMP";
        case PUMP_COMMAND_PRIME_LINE:
            return "PRIME_LINE";
        case PUMP_COMMAND_RUN_INFUSION:
            return "RUN_INFUSION";
        case PUMP_COMMAND_KEEP_VEIN_OPEN:
            return "KEEP_VEIN_OPEN";
        case PUMP_COMMAND_PAUSE_INFUSION:
            return "PAUSE_INFUSION";
        case PUMP_COMMAND_STOP_AND_ALARM:
            return "STOP_AND_ALARM";
        case PUMP_COMMAND_RESET_PUMP:
            return "RESET_PUMP";
        default:
            return "UNKNOWN";
    }
}

const char *pump_fault_name(pump_fault_t fault) {
    switch (fault) {
        case PUMP_FAULT_NONE:
            return "NONE";
        case PUMP_FAULT_OCCLUSION:
            return "OCCLUSION";
        case PUMP_FAULT_AIR_IN_LINE:
            return "AIR_IN_LINE";
        case PUMP_FAULT_DOOR_OPEN:
            return "DOOR_OPEN";
        case PUMP_FAULT_EMPTY_RESERVOIR:
            return "EMPTY_RESERVOIR";
        default:
            return "UNKNOWN";
    }
}
