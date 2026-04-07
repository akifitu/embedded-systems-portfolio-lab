#ifndef LIFT_STATION_TYPES_H
#define LIFT_STATION_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    LIFT_STATE_IDLE = 0,
    LIFT_STATE_DRAIN_LEAD,
    LIFT_STATE_DRAIN_ASSIST,
    LIFT_STATE_FAULT
} lift_state_t;

typedef enum {
    LIFT_COMMAND_STOP_ALL = 0,
    LIFT_COMMAND_RUN_A,
    LIFT_COMMAND_RUN_B,
    LIFT_COMMAND_RUN_BOTH
} lift_command_t;

typedef enum {
    LIFT_FAULT_NONE = 0,
    LIFT_FAULT_OVERFLOW,
    LIFT_FAULT_NO_PUMP_AVAILABLE,
    LIFT_FAULT_PUMP_A_SEAL,
    LIFT_FAULT_PUMP_B_SEAL
} lift_fault_t;

typedef enum {
    LIFT_PUMP_A = 0,
    LIFT_PUMP_B
} lift_pump_t;

typedef struct {
    uint8_t level_pct;
    uint16_t inflow_lpm;
    bool pump_a_ready;
    bool pump_b_ready;
    bool pump_a_seal_ok;
    bool pump_b_seal_ok;
    bool reset_request;
} lift_input_t;

typedef struct {
    lift_state_t state;
    lift_command_t command;
    lift_fault_t fault;
    lift_pump_t lead_pump;
} lift_output_t;

const char *lift_state_name(lift_state_t state);
const char *lift_command_name(lift_command_t command);
const char *lift_fault_name(lift_fault_t fault);
const char *lift_pump_name(lift_pump_t pump);

#endif
