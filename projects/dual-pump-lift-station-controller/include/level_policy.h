#ifndef LEVEL_POLICY_H
#define LEVEL_POLICY_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool start_required;
    bool stop_allowed;
    bool assist_required;
    bool overflow;
} level_policy_t;

level_policy_t level_policy_evaluate(uint8_t level_pct);

#endif
