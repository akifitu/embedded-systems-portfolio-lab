#include "level_policy.h"

#define LIFT_START_LEVEL_PCT 65u
#define LIFT_STOP_LEVEL_PCT 35u
#define LIFT_ASSIST_LEVEL_PCT 85u
#define LIFT_OVERFLOW_LEVEL_PCT 95u

level_policy_t level_policy_evaluate(uint8_t level_pct) {
    level_policy_t policy;

    policy.start_required = level_pct >= LIFT_START_LEVEL_PCT;
    policy.stop_allowed = level_pct <= LIFT_STOP_LEVEL_PCT;
    policy.assist_required = level_pct >= LIFT_ASSIST_LEVEL_PCT;
    policy.overflow = level_pct >= LIFT_OVERFLOW_LEVEL_PCT;
    return policy;
}
