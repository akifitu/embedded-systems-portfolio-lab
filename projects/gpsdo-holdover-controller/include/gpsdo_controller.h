#ifndef GPSDO_CONTROLLER_H
#define GPSDO_CONTROLLER_H

#include "drift_model.h"
#include "gpsdo_types.h"

typedef struct {
    gpsdo_state_t state;
    gpsdo_quality_t quality;
    uint16_t trim_code;
    uint8_t lock_ticks;
    uint32_t holdover_seconds;
    int32_t last_phase_ns;
    drift_model_t drift_model;
} gpsdo_controller_t;

void gpsdo_controller_init(gpsdo_controller_t *controller,
                           int16_t initial_temp_c_x10);
gpsdo_output_t gpsdo_controller_step(gpsdo_controller_t *controller,
                                     const gpsdo_input_t *input);

#endif
