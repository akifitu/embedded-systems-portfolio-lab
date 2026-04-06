#include "motion_classifier.h"

#include <math.h>

iae_motion_state_t iae_classify_motion(const iae_sample_t *sample,
                                       float accel_norm_g,
                                       const iae_output_t *estimate) {
    float gyro_mag =
        fabsf(sample->gyro_dps[0]) + fabsf(sample->gyro_dps[1]) + fabsf(sample->gyro_dps[2]);

    if (accel_norm_g < 0.20f) {
        return IAE_STATE_FREEFALL;
    }

    if (accel_norm_g > 1.12f || gyro_mag > 65.0f) {
        return IAE_STATE_VIBRATION;
    }

    if (fabsf(estimate->roll_deg) > 5.0f || fabsf(estimate->pitch_deg) > 5.0f ||
        gyro_mag > 6.0f) {
        return IAE_STATE_TRACKING;
    }

    return IAE_STATE_STABLE;
}

