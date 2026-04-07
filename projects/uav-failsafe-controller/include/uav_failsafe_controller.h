#ifndef UAV_FAILSAFE_CONTROLLER_H
#define UAV_FAILSAFE_CONTROLLER_H

#include <stdint.h>

#include "failsafe_types.h"

typedef struct {
    uav_state_t state;
    uint8_t link_loss_count;
    uint8_t nav_loss_count;
    uav_reason_t latched_reason;
} uav_failsafe_controller_t;

void uav_failsafe_controller_init(uav_failsafe_controller_t *controller);
uav_output_t uav_failsafe_controller_step(uav_failsafe_controller_t *controller,
                                          const uav_input_t *input);

#endif
