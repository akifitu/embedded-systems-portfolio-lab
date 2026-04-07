#ifndef PUMP_TYPES_H
#define PUMP_TYPES_H

#include <stdbool.h>

typedef enum {
    PUMP_STATE_IDLE = 0,
    PUMP_STATE_PRIMING,
    PUMP_STATE_INFUSING,
    PUMP_STATE_PAUSED,
    PUMP_STATE_KVO,
    PUMP_STATE_ALARM
} pump_state_t;

typedef enum {
    PUMP_COMMAND_HOLD_PUMP = 0,
    PUMP_COMMAND_PRIME_LINE,
    PUMP_COMMAND_RUN_INFUSION,
    PUMP_COMMAND_KEEP_VEIN_OPEN,
    PUMP_COMMAND_PAUSE_INFUSION,
    PUMP_COMMAND_STOP_AND_ALARM,
    PUMP_COMMAND_RESET_PUMP
} pump_command_t;

typedef enum {
    PUMP_FAULT_NONE = 0,
    PUMP_FAULT_OCCLUSION,
    PUMP_FAULT_AIR_IN_LINE,
    PUMP_FAULT_DOOR_OPEN,
    PUMP_FAULT_EMPTY_RESERVOIR
} pump_fault_t;

typedef struct {
    bool prime_request;
    bool start_request;
    bool pause_request;
    bool resume_request;
    bool reset_request;
    bool door_closed;
    bool occlusion_detected;
    bool air_in_line;
    unsigned reservoir_ml_tenths;
    unsigned line_pressure_kpa;
} pump_input_t;

typedef struct {
    pump_state_t state;
    pump_command_t command;
    pump_fault_t fault;
    bool motor_active;
    bool clamp_open;
    bool buzzer_active;
    unsigned rate_mlph;
    unsigned delivered_ml_tenths;
    unsigned therapy_remaining_ml_tenths;
    unsigned reservoir_ml_tenths;
    unsigned line_pressure_kpa;
} pump_output_t;

const char *pump_state_name(pump_state_t state);
const char *pump_command_name(pump_command_t command);
const char *pump_fault_name(pump_fault_t fault);

#endif
