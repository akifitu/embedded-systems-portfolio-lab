#ifndef BATTERY_SWAP_DOCK_CONTROLLER_H
#define BATTERY_SWAP_DOCK_CONTROLLER_H

#include "swap_dock_types.h"

typedef struct {
    swap_dock_state_t state;
    swap_dock_fault_t latched_fault;
    unsigned precharge_frames;
} battery_swap_dock_controller_t;

void battery_swap_dock_controller_init(
    battery_swap_dock_controller_t *controller);
swap_dock_output_t battery_swap_dock_controller_step(
    battery_swap_dock_controller_t *controller,
    const swap_dock_input_t *input);

#endif
