#ifndef PUMP_ARBITER_H
#define PUMP_ARBITER_H

#include <stdbool.h>

#include "lift_station_types.h"

typedef struct {
    bool lead_available;
    bool lag_available;
    lift_pump_t lead;
    lift_pump_t lag;
} pump_selection_t;

pump_selection_t pump_arbiter_select(lift_pump_t preferred_lead, bool pump_a_ready,
                                     bool pump_b_ready, bool pump_a_seal_ok,
                                     bool pump_b_seal_ok);

#endif
