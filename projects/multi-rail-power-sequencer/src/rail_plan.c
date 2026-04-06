#include "rail_plan.h"

static const rail_plan_entry_t k_plan[PSEQ_RAIL_COUNT] = {
    {"CORE_1V0", 5u},
    {"IO_1V8", 4u},
    {"AUX_3V3", 4u},
    {"RADIO_5V", 6u},
};

const rail_plan_entry_t *rail_plan_entry(int rail_index) {
    if (rail_index < 0 || rail_index >= (int)PSEQ_RAIL_COUNT) {
        return 0;
    }

    return &k_plan[rail_index];
}
