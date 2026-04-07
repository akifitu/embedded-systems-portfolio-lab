#ifndef EVSE_TYPES_H
#define EVSE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    EVSE_STATE_IDLE = 0,
    EVSE_STATE_CONNECTED,
    EVSE_STATE_ARMING,
    EVSE_STATE_CHARGING,
    EVSE_STATE_FAULT,
    EVSE_STATE_COOLDOWN
} evse_state_t;

typedef enum {
    EVSE_COMMAND_OPEN_CONTACTOR = 0,
    EVSE_COMMAND_ADVERTISE_CURRENT,
    EVSE_COMMAND_CLOSE_CONTACTOR,
    EVSE_COMMAND_ENERGIZE_PORT,
    EVSE_COMMAND_HOLD_COOLDOWN
} evse_command_t;

typedef enum {
    EVSE_PILOT_A = 0,
    EVSE_PILOT_B,
    EVSE_PILOT_C,
    EVSE_PILOT_F
} evse_pilot_state_t;

typedef enum {
    EVSE_FAULT_NONE = 0,
    EVSE_FAULT_PILOT,
    EVSE_FAULT_GFCI,
    EVSE_FAULT_OVERTEMP,
    EVSE_FAULT_CONTACTOR
} evse_fault_t;

typedef struct {
    int16_t pilot_voltage_dv;
    uint16_t pwm_duty_pct_x10;
    uint16_t cable_limit_a_x10;
    int16_t temp_c_x10;
    bool gfci_ok;
    bool contactor_closed_fb;
} evse_input_t;

typedef struct {
    evse_state_t state;
    evse_command_t command;
    evse_pilot_state_t pilot_state;
    evse_fault_t fault;
    uint16_t current_limit_a_x10;
    uint8_t cooldown_remaining;
    bool contactor_closed_commanded;
} evse_output_t;

const char *evse_state_name(evse_state_t state);
const char *evse_command_name(evse_command_t command);
const char *evse_pilot_name(evse_pilot_state_t pilot_state);
const char *evse_fault_name(evse_fault_t fault);

#endif
