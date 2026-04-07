#ifndef DOSE_ENGINE_H
#define DOSE_ENGINE_H

#include <stdbool.h>

typedef struct {
    unsigned delivered_step_tenths;
    unsigned therapy_remaining_tenths;
    bool therapy_complete;
} dose_engine_result_t;

dose_engine_result_t dose_engine_advance(unsigned therapy_remaining_tenths,
                                         unsigned infusion_step_tenths);

#endif
