#include "attitude_estimator.h"

#include "motion_classifier.h"

void iae_estimator_init(iae_estimator_t *estimator) {
    iae_filter_init(&estimator->filter);
}

iae_output_t iae_estimator_step(iae_estimator_t *estimator,
                                const iae_sample_t *sample) {
    iae_output_t output = {0};

    iae_filter_step(&estimator->filter, sample, &output.accel_norm_g,
                    &output.confidence_percent);
    output.roll_deg = estimator->filter.roll_deg;
    output.pitch_deg = estimator->filter.pitch_deg;
    output.motion_state = iae_classify_motion(sample, output.accel_norm_g, &output);
    return output;
}

