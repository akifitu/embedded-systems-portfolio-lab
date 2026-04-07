#include "swap_dock_types.h"

const char *swap_dock_state_name(swap_dock_state_t state) {
    switch (state) {
        case SWAP_DOCK_STATE_IDLE:
            return "IDLE";
        case SWAP_DOCK_STATE_VALIDATE:
            return "VALIDATE";
        case SWAP_DOCK_STATE_PRECHARGE:
            return "PRECHARGE";
        case SWAP_DOCK_STATE_DOCKED:
            return "DOCKED";
        case SWAP_DOCK_STATE_RELEASE:
            return "RELEASE";
        case SWAP_DOCK_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

const char *swap_dock_command_name(swap_dock_command_t command) {
    switch (command) {
        case SWAP_DOCK_COMMAND_OPEN_DOCK:
            return "OPEN_DOCK";
        case SWAP_DOCK_COMMAND_VERIFY_PACK:
            return "VERIFY_PACK";
        case SWAP_DOCK_COMMAND_PRECHARGE_BUS:
            return "PRECHARGE_BUS";
        case SWAP_DOCK_COMMAND_CLOSE_MAIN_PATH:
            return "CLOSE_MAIN_PATH";
        case SWAP_DOCK_COMMAND_RELEASE_PACK:
            return "RELEASE_PACK";
        case SWAP_DOCK_COMMAND_LATCH_FAULT:
            return "LATCH_FAULT";
        case SWAP_DOCK_COMMAND_RESET_DOCK:
            return "RESET_DOCK";
        default:
            return "UNKNOWN";
    }
}

const char *swap_dock_fault_name(swap_dock_fault_t fault) {
    switch (fault) {
        case SWAP_DOCK_FAULT_NONE:
            return "NONE";
        case SWAP_DOCK_FAULT_MISALIGNED:
            return "MISALIGNED";
        case SWAP_DOCK_FAULT_AUTH_FAIL:
            return "AUTH_FAIL";
        case SWAP_DOCK_FAULT_VOLTAGE_MISMATCH:
            return "VOLTAGE_MISMATCH";
        case SWAP_DOCK_FAULT_OVER_TEMP:
            return "OVER_TEMP";
        case SWAP_DOCK_FAULT_PRECHARGE_TIMEOUT:
            return "PRECHARGE_TIMEOUT";
        default:
            return "UNKNOWN";
    }
}
