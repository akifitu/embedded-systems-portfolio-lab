#ifndef POWER_SEQUENCER_H
#define POWER_SEQUENCER_H

#include <stdint.h>

#include "sequencer_types.h"

typedef struct {
    pseq_state_t state;
    int current_rail;
    uint8_t enabled_mask;
    uint8_t confirmed_mask;
    uint8_t retry_count;
    uint8_t wait_ticks;
    uint8_t cooldown_ticks;
    uint32_t faults;
} power_sequencer_t;

void power_sequencer_init(power_sequencer_t *sequencer);
pseq_output_t power_sequencer_step(power_sequencer_t *sequencer,
                                   const pseq_command_t *command,
                                   const pseq_observation_t *observation);

#endif
