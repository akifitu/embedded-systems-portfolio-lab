#ifndef FEATURE_EXTRACTOR_H
#define FEATURE_EXTRACTOR_H

#include <stddef.h>

#include "monitor_types.h"

void mcm_features_compute(const mcm_sample_t *samples, size_t count,
                          mcm_features_t *features_out);

#endif

