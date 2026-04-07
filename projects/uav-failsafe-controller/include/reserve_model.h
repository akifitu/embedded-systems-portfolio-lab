#ifndef RESERVE_MODEL_H
#define RESERVE_MODEL_H

#include <stdbool.h>
#include <stdint.h>

#include "failsafe_types.h"

typedef struct {
    uint8_t return_budget_pct;
    uint8_t reserve_margin_pct;
    bool reserve_safe;
} reserve_estimate_t;

reserve_estimate_t reserve_model_estimate(const uav_input_t *input);

#endif
