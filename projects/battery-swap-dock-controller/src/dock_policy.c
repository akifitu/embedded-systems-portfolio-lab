#include "dock_policy.h"

#define DOCK_FAN_ON_TEMP_C 65u

dock_policy_output_t dock_policy_apply(swap_dock_state_t state,
                                       const swap_dock_input_t *input,
                                       swap_dock_fault_t fault,
                                       unsigned delta_mv) {
    dock_policy_output_t output;

    output.latch_closed = false;
    output.main_closed = false;
    output.precharge_on = false;
    output.cooling_fan =
        (input->dock_temp_c >= DOCK_FAN_ON_TEMP_C) ||
        (fault == SWAP_DOCK_FAULT_OVER_TEMP);
    output.progress_pct = 0u;

    switch (state) {
        case SWAP_DOCK_STATE_IDLE:
            break;

        case SWAP_DOCK_STATE_VALIDATE:
            output.progress_pct = 25u;
            break;

        case SWAP_DOCK_STATE_PRECHARGE:
            output.latch_closed = true;
            output.precharge_on = true;
            output.progress_pct = (delta_mv <= 1000u) ? 65u : 50u;
            break;

        case SWAP_DOCK_STATE_DOCKED:
            output.latch_closed = true;
            output.main_closed = true;
            output.progress_pct = 100u;
            break;

        case SWAP_DOCK_STATE_RELEASE:
            break;

        case SWAP_DOCK_STATE_FAULT:
        default:
            break;
    }

    return output;
}
