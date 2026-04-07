#include "reserve_model.h"

static uint8_t clamp_u8(unsigned value, unsigned limit) {
    return (value > limit) ? (uint8_t)limit : (uint8_t)value;
}

reserve_estimate_t reserve_model_estimate(const uav_input_t *input) {
    reserve_estimate_t estimate;
    unsigned required_pct;

    required_pct = 12u;
    required_pct += (unsigned)input->distance_home_m / 35u;
    required_pct += (unsigned)input->altitude_m / 25u;
    required_pct += (unsigned)input->wind_dmps / 10u;

    estimate.return_budget_pct = clamp_u8(required_pct, 95u);
    if (input->battery_pct > estimate.return_budget_pct) {
        estimate.reserve_margin_pct =
            (uint8_t)(input->battery_pct - estimate.return_budget_pct);
    } else {
        estimate.reserve_margin_pct = 0u;
    }

    estimate.reserve_safe =
        (estimate.reserve_margin_pct >= 15u) && (input->battery_pct >= 20u);
    return estimate;
}
