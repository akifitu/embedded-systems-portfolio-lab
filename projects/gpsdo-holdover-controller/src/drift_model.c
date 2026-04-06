#include "drift_model.h"

static int16_t abs_i16(int16_t value) {
    return value < 0 ? (int16_t)-value : value;
}

void drift_model_init(drift_model_t *model, int16_t initial_temp_c_x10) {
    model->anchor_temp_c_x10 = initial_temp_c_x10;
    model->slope_trim_per_c_x10 = 8;
    model->last_temp_c_x10 = initial_temp_c_x10;
}

void drift_model_update_locked(drift_model_t *model, int16_t trim_delta,
                               int16_t temperature_c_x10) {
    int16_t temp_delta_c_x10 =
        (int16_t)(temperature_c_x10 - model->last_temp_c_x10);

    if (temp_delta_c_x10 != 0) {
        model->slope_trim_per_c_x10 =
            (int16_t)((model->slope_trim_per_c_x10 * 3 + trim_delta * 10) / 4);
    }
    model->last_temp_c_x10 = temperature_c_x10;
}

int16_t drift_model_holdover_trim(const drift_model_t *model,
                                  int16_t temperature_c_x10) {
    int32_t temp_delta_c_x10 =
        (int32_t)temperature_c_x10 - (int32_t)model->anchor_temp_c_x10;

    return (int16_t)((temp_delta_c_x10 * model->slope_trim_per_c_x10) / 100);
}

uint32_t drift_model_uncertainty(const drift_model_t *model,
                                 int16_t temperature_c_x10,
                                 uint32_t holdover_seconds) {
    uint32_t temp_component =
        (uint32_t)(abs_i16((int16_t)(temperature_c_x10 - model->anchor_temp_c_x10)) *
                   6u);
    uint32_t time_component = holdover_seconds * 3u;
    uint32_t slope_component = (uint32_t)(abs_i16(model->slope_trim_per_c_x10) * 2u);

    return temp_component + time_component + slope_component;
}
