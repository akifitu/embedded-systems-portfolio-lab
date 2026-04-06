#include <math.h>
#include <stdio.h>

#include "attitude_estimator.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static int test_level_stays_near_zero(void) {
    iae_estimator_t estimator;
    iae_output_t output = {0};
    iae_sample_t sample = {{0.0f, 0.0f, 1.0f}, {0.1f, -0.1f, 0.0f}, 0.02f};

    iae_estimator_init(&estimator);
    for (int i = 0; i < 20; ++i) {
        output = iae_estimator_step(&estimator, &sample);
    }

    ASSERT_TRUE(fabsf(output.roll_deg) < 1.0f);
    ASSERT_TRUE(fabsf(output.pitch_deg) < 1.0f);
    ASSERT_TRUE(output.motion_state == IAE_STATE_STABLE);
    return 0;
}

static int test_tilt_converges_pitch(void) {
    iae_estimator_t estimator;
    iae_output_t output = {0};
    iae_sample_t sample = {{-0.50f, 0.0f, 0.866f}, {0.0f, 0.0f, 0.0f}, 0.02f};

    iae_estimator_init(&estimator);
    for (int i = 0; i < 20; ++i) {
        output = iae_estimator_step(&estimator, &sample);
    }

    ASSERT_TRUE(output.pitch_deg > 25.0f && output.pitch_deg < 35.0f);
    ASSERT_TRUE(output.motion_state == IAE_STATE_TRACKING);
    return 0;
}

static int test_vibration_state_detected(void) {
    iae_estimator_t estimator;
    iae_output_t output;
    iae_sample_t sample = {{-0.45f, 0.35f, 1.04f}, {28.0f, -22.0f, 20.0f}, 0.02f};

    iae_estimator_init(&estimator);
    output = iae_estimator_step(&estimator, &sample);
    ASSERT_TRUE(output.motion_state == IAE_STATE_VIBRATION);
    ASSERT_TRUE(output.confidence_percent < 90u);
    return 0;
}

static int test_freefall_detected(void) {
    iae_estimator_t estimator;
    iae_output_t output;
    iae_sample_t sample = {{0.02f, -0.03f, 0.06f}, {5.0f, 4.0f, 1.0f}, 0.02f};

    iae_estimator_init(&estimator);
    output = iae_estimator_step(&estimator, &sample);
    ASSERT_TRUE(output.motion_state == IAE_STATE_FREEFALL);
    ASSERT_TRUE(output.accel_norm_g < 0.20f);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_level_stays_near_zero() == 0);
    ASSERT_TRUE(test_tilt_converges_pitch() == 0);
    ASSERT_TRUE(test_vibration_state_detected() == 0);
    ASSERT_TRUE(test_freefall_detected() == 0);
    printf("imu-attitude-estimator tests passed\n");
    return 0;
}
