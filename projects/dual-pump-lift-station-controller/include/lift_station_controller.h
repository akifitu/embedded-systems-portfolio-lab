#ifndef LIFT_STATION_CONTROLLER_H
#define LIFT_STATION_CONTROLLER_H

#include "lift_station_types.h"

typedef struct {
    lift_state_t state;
    lift_fault_t latched_fault;
    lift_pump_t preferred_lead;
} lift_station_controller_t;

void lift_station_controller_init(lift_station_controller_t *controller);
lift_output_t lift_station_controller_step(lift_station_controller_t *controller,
                                           const lift_input_t *input);

#endif
