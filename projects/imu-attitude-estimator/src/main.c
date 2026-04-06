#include <stdio.h>

#include "attitude_estimator.h"

typedef struct {
    const char *label;
    iae_sample_t sample;
    uint8_t repeats;
} iae_phase_t;

static void print_phase(const char *label, const iae_output_t *output) {
    printf("phase=%s roll=%.1f pitch=%.1f accel_norm=%.2f state=%s confidence=%u\n",
           label, output->roll_deg, output->pitch_deg, output->accel_norm_g,
           iae_motion_state_name(output->motion_state), output->confidence_percent);
}

int main(void) {
    iae_estimator_t estimator;
    iae_output_t output = {0};
    const iae_phase_t phases[] = {
        {"level", {{0.0f, 0.0f, 1.0f}, {0.5f, 0.2f, 0.0f}, 0.02f}, 20u},
        {"tilt", {{-0.48f, 0.00f, 0.88f}, {0.8f, 1.2f, 0.0f}, 0.02f}, 20u},
        {"vibration", {{-0.45f, 0.35f, 1.04f}, {28.0f, -22.0f, 20.0f}, 0.02f}, 10u},
        {"freefall", {{0.04f, -0.03f, 0.07f}, {15.0f, 10.0f, 3.0f}, 0.02f}, 5u},
        {"recovery", {{-0.07f, 0.01f, 1.00f}, {-3.0f, -18.0f, 1.0f}, 0.02f}, 18u},
    };

    iae_estimator_init(&estimator);

    for (size_t i = 0; i < (sizeof(phases) / sizeof(phases[0])); ++i) {
        for (uint8_t j = 0u; j < phases[i].repeats; ++j) {
            output = iae_estimator_step(&estimator, &phases[i].sample);
        }
        print_phase(phases[i].label, &output);
    }

    return 0;
}

