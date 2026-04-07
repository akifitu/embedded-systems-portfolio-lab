#ifndef DOCK_POLICY_H
#define DOCK_POLICY_H

#include "swap_dock_types.h"

typedef struct {
    bool latch_closed;
    bool main_closed;
    bool precharge_on;
    bool cooling_fan;
    unsigned progress_pct;
} dock_policy_output_t;

dock_policy_output_t dock_policy_apply(swap_dock_state_t state,
                                       const swap_dock_input_t *input,
                                       swap_dock_fault_t fault,
                                       unsigned delta_mv);

#endif
