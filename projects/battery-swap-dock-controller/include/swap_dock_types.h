#ifndef SWAP_DOCK_TYPES_H
#define SWAP_DOCK_TYPES_H

#include <stdbool.h>

typedef enum {
    SWAP_DOCK_STATE_IDLE = 0,
    SWAP_DOCK_STATE_VALIDATE,
    SWAP_DOCK_STATE_PRECHARGE,
    SWAP_DOCK_STATE_DOCKED,
    SWAP_DOCK_STATE_RELEASE,
    SWAP_DOCK_STATE_FAULT
} swap_dock_state_t;

typedef enum {
    SWAP_DOCK_COMMAND_OPEN_DOCK = 0,
    SWAP_DOCK_COMMAND_VERIFY_PACK,
    SWAP_DOCK_COMMAND_PRECHARGE_BUS,
    SWAP_DOCK_COMMAND_CLOSE_MAIN_PATH,
    SWAP_DOCK_COMMAND_RELEASE_PACK,
    SWAP_DOCK_COMMAND_LATCH_FAULT,
    SWAP_DOCK_COMMAND_RESET_DOCK
} swap_dock_command_t;

typedef enum {
    SWAP_DOCK_FAULT_NONE = 0,
    SWAP_DOCK_FAULT_MISALIGNED,
    SWAP_DOCK_FAULT_AUTH_FAIL,
    SWAP_DOCK_FAULT_VOLTAGE_MISMATCH,
    SWAP_DOCK_FAULT_OVER_TEMP,
    SWAP_DOCK_FAULT_PRECHARGE_TIMEOUT
} swap_dock_fault_t;

typedef struct {
    bool pack_present;
    bool enable_request;
    bool release_request;
    bool reset_request;
    bool aligned;
    bool auth_ok;
    unsigned pack_voltage_mv;
    unsigned bus_voltage_mv;
    unsigned dock_temp_c;
} swap_dock_input_t;

typedef struct {
    swap_dock_state_t state;
    swap_dock_command_t command;
    swap_dock_fault_t fault;
    bool pack_present;
    bool latch_closed;
    bool main_closed;
    bool precharge_on;
    bool cooling_fan;
    unsigned delta_mv;
    unsigned progress_pct;
    unsigned pack_voltage_mv;
    unsigned bus_voltage_mv;
    unsigned dock_temp_c;
} swap_dock_output_t;

const char *swap_dock_state_name(swap_dock_state_t state);
const char *swap_dock_command_name(swap_dock_command_t command);
const char *swap_dock_fault_name(swap_dock_fault_t fault);

#endif
