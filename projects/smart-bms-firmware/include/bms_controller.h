#ifndef BMS_CONTROLLER_H
#define BMS_CONTROLLER_H

#include "bms_types.h"
#include "soc_estimator.h"

typedef struct {
    soc_estimator_t soc;
    bms_state_t state;
} bms_controller_t;

void bms_controller_init(bms_controller_t *controller, float pack_capacity_ah,
                         float initial_soc_percent);
bms_output_t bms_controller_step(bms_controller_t *controller,
                                 const bms_sample_t *sample);
const char *bms_state_name(bms_state_t state);

#endif

