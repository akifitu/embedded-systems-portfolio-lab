#include "energy_budget.h"

static uint16_t action_cost_mwh(ehn_action_t action) {
    switch (action) {
        case EHN_ACTION_SLEEP:
            return 10u;
        case EHN_ACTION_SAMPLE:
            return 80u;
        case EHN_ACTION_TX:
            return 260u;
        case EHN_ACTION_GNSS:
            return 420u;
        case EHN_ACTION_DEEP_SLEEP:
            return 4u;
        default:
            return 0u;
    }
}

uint16_t ehn_energy_apply(uint16_t battery_mwh, uint16_t harvested_mw,
                          ehn_action_t action, uint16_t wake_seconds) {
    uint32_t harvested_mwh = ((uint32_t)harvested_mw * (uint32_t)wake_seconds) / 3600u;
    uint32_t available = (uint32_t)battery_mwh + harvested_mwh;
    uint16_t cost = action_cost_mwh(action);

    if (available <= cost) {
        return 0u;
    }

    available -= cost;
    if (available > 8000u) {
        available = 8000u;
    }
    return (uint16_t)available;
}

