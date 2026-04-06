#ifndef NODE_TYPES_H
#define NODE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    EHN_ACTION_SLEEP = 0,
    EHN_ACTION_SAMPLE,
    EHN_ACTION_TX,
    EHN_ACTION_GNSS,
    EHN_ACTION_DEEP_SLEEP
} ehn_action_t;

typedef enum {
    EHN_MODE_PERFORMANCE = 0,
    EHN_MODE_BALANCED,
    EHN_MODE_SURVIVAL
} ehn_mode_t;

typedef struct {
    uint16_t battery_mwh;
    uint16_t harvested_mw;
    uint8_t telemetry_backlog;
    uint16_t seconds_since_sample;
    uint16_t seconds_since_tx;
    bool gnss_fix_required;
} ehn_node_input_t;

typedef struct {
    ehn_mode_t mode;
    ehn_action_t action;
    uint16_t next_wake_seconds;
    bool reserve_protected;
} ehn_policy_t;

typedef struct {
    ehn_mode_t mode;
    ehn_action_t action;
    uint16_t battery_mwh;
    uint8_t telemetry_backlog;
    uint16_t next_wake_seconds;
    bool reserve_protected;
} ehn_step_result_t;

const char *ehn_action_name(ehn_action_t action);
const char *ehn_mode_name(ehn_mode_t mode);

#endif

