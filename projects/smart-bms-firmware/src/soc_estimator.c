#include "soc_estimator.h"

static float clampf(float value, float min_value, float max_value) {
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

void soc_estimator_init(soc_estimator_t *estimator, float capacity_ah,
                        float initial_soc_percent) {
    estimator->capacity_ah = capacity_ah;
    estimator->soc_percent = clampf(initial_soc_percent, 0.0f, 100.0f);
}

float soc_estimator_update(soc_estimator_t *estimator, float pack_current_a,
                           float dt_seconds) {
    float delta_ah = (pack_current_a * dt_seconds) / 3600.0f;
    float delta_percent = (delta_ah / estimator->capacity_ah) * 100.0f;

    estimator->soc_percent =
        clampf(estimator->soc_percent - delta_percent, 0.0f, 100.0f);
    return estimator->soc_percent;
}

