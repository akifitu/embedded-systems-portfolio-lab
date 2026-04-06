#ifndef ATTITUDE_ESTIMATOR_H
#define ATTITUDE_ESTIMATOR_H

#include "complementary_filter.h"

typedef struct {
    iae_filter_t filter;
} iae_estimator_t;

void iae_estimator_init(iae_estimator_t *estimator);
iae_output_t iae_estimator_step(iae_estimator_t *estimator,
                                const iae_sample_t *sample);

#endif

