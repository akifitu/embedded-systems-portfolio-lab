#ifndef MOTION_CLASSIFIER_H
#define MOTION_CLASSIFIER_H

#include "imu_types.h"

iae_motion_state_t iae_classify_motion(const iae_sample_t *sample,
                                       float accel_norm_g,
                                       const iae_output_t *estimate);

#endif

