#include "node_controller.h"

#include "energy_budget.h"
#include "task_policy.h"

void ehn_node_init(ehn_node_state_t *state, uint16_t battery_mwh,
                   uint8_t telemetry_backlog) {
    state->battery_mwh = battery_mwh;
    state->telemetry_backlog = telemetry_backlog;
    state->seconds_since_sample = 0u;
    state->seconds_since_tx = 0u;
}

ehn_step_result_t ehn_node_step(ehn_node_state_t *state,
                                const ehn_node_input_t *input) {
    ehn_policy_t policy = ehn_policy_decide(input);
    ehn_step_result_t result = {0};

    state->battery_mwh = input->battery_mwh;
    state->telemetry_backlog = input->telemetry_backlog;
    state->seconds_since_sample = input->seconds_since_sample;
    state->seconds_since_tx = input->seconds_since_tx;

    state->battery_mwh =
        ehn_energy_apply(input->battery_mwh, input->harvested_mw, policy.action,
                         policy.next_wake_seconds);

    state->seconds_since_sample = input->seconds_since_sample + policy.next_wake_seconds;
    state->seconds_since_tx = input->seconds_since_tx + policy.next_wake_seconds;

    if (policy.action == EHN_ACTION_SAMPLE) {
        state->telemetry_backlog = (uint8_t)(input->telemetry_backlog + 1u);
        state->seconds_since_sample = 0u;
    } else if (policy.action == EHN_ACTION_TX) {
        state->telemetry_backlog =
            (input->telemetry_backlog > 0u) ? (uint8_t)(input->telemetry_backlog - 1u) : 0u;
        state->seconds_since_tx = 0u;
    }

    result.mode = policy.mode;
    result.action = policy.action;
    result.battery_mwh = state->battery_mwh;
    result.telemetry_backlog = state->telemetry_backlog;
    result.next_wake_seconds = policy.next_wake_seconds;
    result.reserve_protected = policy.reserve_protected;
    return result;
}
