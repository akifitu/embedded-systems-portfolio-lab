#include "pump_arbiter.h"

static bool pump_ok(lift_pump_t pump, bool pump_a_ready, bool pump_b_ready,
                    bool pump_a_seal_ok, bool pump_b_seal_ok) {
    if (pump == LIFT_PUMP_A) {
        return pump_a_ready && pump_a_seal_ok;
    }
    return pump_b_ready && pump_b_seal_ok;
}

pump_selection_t pump_arbiter_select(lift_pump_t preferred_lead, bool pump_a_ready,
                                     bool pump_b_ready, bool pump_a_seal_ok,
                                     bool pump_b_seal_ok) {
    pump_selection_t selection;

    selection.lead = preferred_lead;
    selection.lag = (preferred_lead == LIFT_PUMP_A) ? LIFT_PUMP_B : LIFT_PUMP_A;
    selection.lead_available =
        pump_ok(selection.lead, pump_a_ready, pump_b_ready, pump_a_seal_ok,
                pump_b_seal_ok);
    selection.lag_available =
        pump_ok(selection.lag, pump_a_ready, pump_b_ready, pump_a_seal_ok,
                pump_b_seal_ok);

    if (!selection.lead_available && selection.lag_available) {
        lift_pump_t tmp;

        tmp = selection.lead;
        selection.lead = selection.lag;
        selection.lag = tmp;
        selection.lead_available = true;
        selection.lag_available =
            pump_ok(selection.lag, pump_a_ready, pump_b_ready, pump_a_seal_ok,
                    pump_b_seal_ok);
    }

    return selection;
}
