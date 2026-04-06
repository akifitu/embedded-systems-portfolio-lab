#ifndef COMPLEMENTARY_FILTER_H
#define COMPLEMENTARY_FILTER_H

#include "imu_types.h"

typedef struct {
    float roll_deg;
    float pitch_deg;
} iae_filter_t;

void iae_filter_init(iae_filter_t *filter);
void iae_filter_step(iae_filter_t *filter, const iae_sample_t *sample,
                     float *accel_norm_out, uint8_t *confidence_out);

#endif

