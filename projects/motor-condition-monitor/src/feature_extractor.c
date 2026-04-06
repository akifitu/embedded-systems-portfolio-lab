#include "feature_extractor.h"

static uint32_t abs_u32(int32_t value) {
    if (value < 0) {
        return (uint32_t)(-value);
    }
    return (uint32_t)value;
}

static uint32_t isqrt_u64(uint64_t value) {
    uint64_t result = 0;
    uint64_t bit = 1ull << 62;

    while (bit > value) {
        bit >>= 2u;
    }

    while (bit != 0u) {
        if (value >= result + bit) {
            value -= result + bit;
            result = (result >> 1u) + bit;
        } else {
            result >>= 1u;
        }
        bit >>= 2u;
    }

    return (uint32_t)result;
}

void mcm_features_compute(const mcm_sample_t *samples, size_t count,
                          mcm_features_t *features_out) {
    uint64_t energy_sum = 0u;
    uint64_t current_sum = 0u;
    uint32_t jerk_sum = 0u;
    int16_t min_vibration;
    int16_t max_vibration;
    int16_t max_temp;

    if (samples == 0 || features_out == 0 || count == 0u) {
        return;
    }

    min_vibration = samples[0].vibration_mg;
    max_vibration = samples[0].vibration_mg;
    max_temp = samples[0].board_temp_deci_c;

    for (size_t i = 0; i < count; ++i) {
        int32_t vibration = samples[i].vibration_mg;

        energy_sum += (uint64_t)((int64_t)vibration * (int64_t)vibration);
        current_sum += samples[i].motor_current_ma;

        if (samples[i].vibration_mg < min_vibration) {
            min_vibration = samples[i].vibration_mg;
        }
        if (samples[i].vibration_mg > max_vibration) {
            max_vibration = samples[i].vibration_mg;
        }
        if (samples[i].board_temp_deci_c > max_temp) {
            max_temp = samples[i].board_temp_deci_c;
        }

        if (i > 0u) {
            jerk_sum += abs_u32((int32_t)samples[i].vibration_mg -
                                (int32_t)samples[i - 1u].vibration_mg);
        }
    }

    features_out->rms_mg = (uint16_t)isqrt_u64(energy_sum / count);
    features_out->peak_to_peak_mg =
        (uint16_t)((int32_t)max_vibration - (int32_t)min_vibration);
    features_out->mean_abs_jerk_mg =
        (count > 1u) ? (uint16_t)(jerk_sum / (uint32_t)(count - 1u)) : 0u;
    features_out->avg_current_ma = (uint16_t)(current_sum / count);
    features_out->max_temp_deci_c = max_temp;
}

