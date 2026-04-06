#ifndef NODE_CONTROLLER_H
#define NODE_CONTROLLER_H

#include "node_types.h"

typedef struct {
    uint16_t battery_mwh;
    uint8_t telemetry_backlog;
    uint16_t seconds_since_sample;
    uint16_t seconds_since_tx;
} ehn_node_state_t;

void ehn_node_init(ehn_node_state_t *state, uint16_t battery_mwh,
                   uint8_t telemetry_backlog);
ehn_step_result_t ehn_node_step(ehn_node_state_t *state,
                                const ehn_node_input_t *input);

#endif

