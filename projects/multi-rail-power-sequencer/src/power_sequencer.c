#include "power_sequencer.h"

#include "rail_plan.h"

enum {
    PSEQ_MAX_RETRIES = 2u,
    PSEQ_RETRY_COOLDOWN_TICKS = 3u
};

static uint8_t rail_bit(int rail_index) {
    return (uint8_t)(1u << rail_index);
}

static uint8_t count_enabled(uint8_t mask) {
    uint8_t count = 0u;

    while (mask != 0u) {
        count = (uint8_t)(count + (mask & 1u));
        mask >>= 1u;
    }
    return count;
}

static uint8_t progress_from_confirmed(uint8_t confirmed_mask, pseq_state_t state) {
    if (state == PSEQ_STATE_IDLE) {
        return 0u;
    }
    if (state == PSEQ_STATE_STABLE) {
        return 100u;
    }

    return (uint8_t)(count_enabled(confirmed_mask) * 100u / PSEQ_RAIL_COUNT);
}

static void start_sequence(power_sequencer_t *sequencer) {
    sequencer->state = PSEQ_STATE_STARTUP;
    sequencer->current_rail = PSEQ_RAIL_CORE_1V0;
    sequencer->enabled_mask = rail_bit(PSEQ_RAIL_CORE_1V0);
    sequencer->confirmed_mask = 0u;
    sequencer->wait_ticks = 0u;
    sequencer->cooldown_ticks = 0u;
    sequencer->faults = PSEQ_FAULT_NONE;
}

static void enter_fault(power_sequencer_t *sequencer, uint32_t fault) {
    sequencer->state = PSEQ_STATE_FAULT;
    sequencer->current_rail = PSEQ_RAIL_NONE;
    sequencer->enabled_mask = 0u;
    sequencer->faults |= fault;
}

static pseq_output_t compose_output(const power_sequencer_t *sequencer) {
    pseq_output_t output;

    output.state = sequencer->state;
    output.current_rail = sequencer->current_rail;
    output.enabled_mask = sequencer->enabled_mask;
    output.retry_count = sequencer->retry_count;
    output.progress_percent =
        progress_from_confirmed(sequencer->confirmed_mask, sequencer->state);
    output.faults = sequencer->faults;
    return output;
}

void power_sequencer_init(power_sequencer_t *sequencer) {
    sequencer->state = PSEQ_STATE_IDLE;
    sequencer->current_rail = PSEQ_RAIL_NONE;
    sequencer->enabled_mask = 0u;
    sequencer->confirmed_mask = 0u;
    sequencer->retry_count = 0u;
    sequencer->wait_ticks = 0u;
    sequencer->cooldown_ticks = 0u;
    sequencer->faults = PSEQ_FAULT_NONE;
}

pseq_output_t power_sequencer_step(power_sequencer_t *sequencer,
                                   const pseq_command_t *command,
                                   const pseq_observation_t *observation) {
    if (command->clear_fault) {
        power_sequencer_init(sequencer);
    }

    if ((sequencer->state == PSEQ_STATE_STARTUP ||
         sequencer->state == PSEQ_STATE_STABLE ||
         sequencer->state == PSEQ_STATE_RETRY_WAIT) &&
        observation->brownout) {
        enter_fault(sequencer, PSEQ_FAULT_BROWNOUT);
        return compose_output(sequencer);
    }

    if ((sequencer->state == PSEQ_STATE_STARTUP ||
         sequencer->state == PSEQ_STATE_STABLE ||
         sequencer->state == PSEQ_STATE_RETRY_WAIT) &&
        observation->short_circuit) {
        enter_fault(sequencer, PSEQ_FAULT_SHORT_CIRCUIT);
        return compose_output(sequencer);
    }

    if (sequencer->state == PSEQ_STATE_IDLE && command->enable_request) {
        start_sequence(sequencer);
    }

    if (sequencer->state == PSEQ_STATE_STABLE && command->shutdown_request) {
        sequencer->state = PSEQ_STATE_SHUTDOWN;
        sequencer->current_rail = (int)PSEQ_RAIL_COUNT - 1;
    }

    switch (sequencer->state) {
        case PSEQ_STATE_IDLE:
            break;
        case PSEQ_STATE_STARTUP: {
            const rail_plan_entry_t *entry = rail_plan_entry(sequencer->current_rail);

            if (entry == 0) {
                enter_fault(sequencer, PSEQ_FAULT_PG_TIMEOUT);
                break;
            }

            if (observation->rail_pg[sequencer->current_rail]) {
                sequencer->confirmed_mask |= rail_bit(sequencer->current_rail);
                sequencer->wait_ticks = 0u;
                sequencer->current_rail++;
                if (sequencer->current_rail >= (int)PSEQ_RAIL_COUNT) {
                    sequencer->state = PSEQ_STATE_STABLE;
                    sequencer->current_rail = PSEQ_RAIL_NONE;
                    sequencer->faults = PSEQ_FAULT_NONE;
                } else {
                    sequencer->enabled_mask |= rail_bit(sequencer->current_rail);
                }
            } else {
                sequencer->wait_ticks++;
                if (sequencer->wait_ticks >= entry->pg_timeout_ticks) {
                    if (sequencer->retry_count < PSEQ_MAX_RETRIES) {
                        sequencer->state = PSEQ_STATE_RETRY_WAIT;
                        sequencer->retry_count++;
                        sequencer->enabled_mask = 0u;
                        sequencer->confirmed_mask = 0u;
                        sequencer->current_rail = PSEQ_RAIL_CORE_1V0;
                        sequencer->wait_ticks = 0u;
                        sequencer->cooldown_ticks = PSEQ_RETRY_COOLDOWN_TICKS;
                        sequencer->faults = PSEQ_FAULT_PG_TIMEOUT;
                    } else {
                        enter_fault(sequencer, PSEQ_FAULT_PG_TIMEOUT |
                                                   PSEQ_FAULT_RETRY_EXHAUSTED);
                    }
                }
            }
            break;
        }
        case PSEQ_STATE_STABLE:
            break;
        case PSEQ_STATE_RETRY_WAIT:
            if (sequencer->cooldown_ticks > 0u) {
                sequencer->cooldown_ticks--;
            }
            if (sequencer->cooldown_ticks == 0u) {
                start_sequence(sequencer);
            }
            break;
        case PSEQ_STATE_SHUTDOWN:
            if (sequencer->current_rail >= 0) {
                sequencer->enabled_mask &=
                    (uint8_t)~rail_bit(sequencer->current_rail);
                sequencer->current_rail--;
            }

            if (sequencer->enabled_mask == 0u) {
                power_sequencer_init(sequencer);
            }
            break;
        case PSEQ_STATE_FAULT:
            break;
    }

    return compose_output(sequencer);
}
