#include "complementary_filter.h"

#include <math.h>

static float clampf(float value, float min_value, float max_value) {
    if (value < min_value) {
        return min_value;
    }
    if (value > max_value) {
        return max_value;
    }
    return value;
}

static float degrees_from_radians(float radians) {
    return radians * (180.0f / 3.14159265358979323846f);
}

void iae_filter_init(iae_filter_t *filter) {
    filter->roll_deg = 0.0f;
    filter->pitch_deg = 0.0f;
}

void iae_filter_step(iae_filter_t *filter, const iae_sample_t *sample,
                     float *accel_norm_out, uint8_t *confidence_out) {
    float ax = sample->accel_g[0];
    float ay = sample->accel_g[1];
    float az = sample->accel_g[2];
    float accel_norm = sqrtf(ax * ax + ay * ay + az * az);
    float accel_roll_deg = degrees_from_radians(atan2f(ay, az));
    float accel_pitch_deg =
        degrees_from_radians(atan2f(-ax, sqrtf(ay * ay + az * az)));
    float accel_error = fabsf(accel_norm - 1.0f);
    float accel_weight = clampf(1.0f - (accel_error * 2.0f), 0.10f, 0.96f);

    filter->roll_deg += sample->gyro_dps[0] * sample->dt_s;
    filter->pitch_deg += sample->gyro_dps[1] * sample->dt_s;

    filter->roll_deg =
        (filter->roll_deg * (1.0f - accel_weight)) + (accel_roll_deg * accel_weight);
    filter->pitch_deg =
        (filter->pitch_deg * (1.0f - accel_weight)) + (accel_pitch_deg * accel_weight);

    *accel_norm_out = accel_norm;
    *confidence_out = (uint8_t)(accel_weight * 100.0f + 0.5f);
}

