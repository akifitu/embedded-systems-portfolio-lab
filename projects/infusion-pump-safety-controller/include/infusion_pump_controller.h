#ifndef INFUSION_PUMP_CONTROLLER_H
#define INFUSION_PUMP_CONTROLLER_H

#include "pump_types.h"

typedef struct {
    pump_state_t state;
    pump_fault_t latched_fault;
    unsigned delivered_ml_tenths;
    unsigned therapy_remaining_ml_tenths;
} infusion_pump_controller_t;

void infusion_pump_controller_init(infusion_pump_controller_t *controller);
pump_output_t infusion_pump_controller_step(infusion_pump_controller_t *controller,
                                            const pump_input_t *input);

#endif
