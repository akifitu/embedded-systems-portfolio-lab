#include "evse_types.h"

const char *evse_state_name(evse_state_t state) {
    switch (state) {
        case EVSE_STATE_IDLE:
            return "IDLE";
        case EVSE_STATE_CONNECTED:
            return "CONNECTED";
        case EVSE_STATE_ARMING:
            return "ARMING";
        case EVSE_STATE_CHARGING:
            return "CHARGING";
        case EVSE_STATE_FAULT:
            return "FAULT";
        case EVSE_STATE_COOLDOWN:
            return "COOLDOWN";
        default:
            return "UNKNOWN";
    }
}

const char *evse_command_name(evse_command_t command) {
    switch (command) {
        case EVSE_COMMAND_OPEN_CONTACTOR:
            return "OPEN_CONTACTOR";
        case EVSE_COMMAND_ADVERTISE_CURRENT:
            return "ADVERTISE_CURRENT";
        case EVSE_COMMAND_CLOSE_CONTACTOR:
            return "CLOSE_CONTACTOR";
        case EVSE_COMMAND_ENERGIZE_PORT:
            return "ENERGIZE_PORT";
        case EVSE_COMMAND_HOLD_COOLDOWN:
            return "HOLD_COOLDOWN";
        default:
            return "UNKNOWN";
    }
}

const char *evse_pilot_name(evse_pilot_state_t pilot_state) {
    switch (pilot_state) {
        case EVSE_PILOT_A:
            return "A";
        case EVSE_PILOT_B:
            return "B";
        case EVSE_PILOT_C:
            return "C";
        case EVSE_PILOT_F:
            return "F";
        default:
            return "?";
    }
}

const char *evse_fault_name(evse_fault_t fault) {
    switch (fault) {
        case EVSE_FAULT_NONE:
            return "NONE";
        case EVSE_FAULT_PILOT:
            return "PILOT";
        case EVSE_FAULT_GFCI:
            return "GFCI";
        case EVSE_FAULT_OVERTEMP:
            return "OVERTEMP";
        case EVSE_FAULT_CONTACTOR:
            return "CONTACTOR";
        default:
            return "UNKNOWN";
    }
}
