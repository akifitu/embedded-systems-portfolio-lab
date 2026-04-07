#ifndef EVSE_CONTROLLER_H
#define EVSE_CONTROLLER_H

#include "evse_types.h"

typedef struct {
    evse_state_t state;
    uint8_t cooldown_remaining;
    evse_fault_t latched_fault;
} evse_controller_t;

void evse_controller_init(evse_controller_t *controller);
evse_output_t evse_controller_step(evse_controller_t *controller,
                                   const evse_input_t *input);

#endif
