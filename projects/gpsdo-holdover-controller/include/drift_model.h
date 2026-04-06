#ifndef DRIFT_MODEL_H
#define DRIFT_MODEL_H

#include <stdint.h>

typedef struct {
    int16_t anchor_temp_c_x10;
    int16_t slope_trim_per_c_x10;
    int16_t last_temp_c_x10;
} drift_model_t;

void drift_model_init(drift_model_t *model, int16_t initial_temp_c_x10);
void drift_model_update_locked(drift_model_t *model, int16_t trim_delta,
                               int16_t temperature_c_x10);
int16_t drift_model_holdover_trim(const drift_model_t *model,
                                  int16_t temperature_c_x10);
uint32_t drift_model_uncertainty(const drift_model_t *model,
                                 int16_t temperature_c_x10,
                                 uint32_t holdover_seconds);

#endif
