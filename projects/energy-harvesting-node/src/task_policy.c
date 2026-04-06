#include "task_policy.h"

static bool reserve_protected(const ehn_node_input_t *input) {
    return input->battery_mwh <= 2200u;
}

ehn_policy_t ehn_policy_decide(const ehn_node_input_t *input) {
    ehn_policy_t policy = {0};

    policy.reserve_protected = reserve_protected(input);

    if (policy.reserve_protected) {
        policy.mode = EHN_MODE_SURVIVAL;
        policy.action = EHN_ACTION_DEEP_SLEEP;
        policy.next_wake_seconds = 60u;
        return policy;
    }

    if (input->battery_mwh >= 5600u && input->harvested_mw >= 900u) {
        policy.mode = EHN_MODE_PERFORMANCE;
    } else {
        policy.mode = EHN_MODE_BALANCED;
    }

    if (input->gnss_fix_required && input->battery_mwh >= 6200u &&
        input->harvested_mw >= 1100u) {
        policy.action = EHN_ACTION_GNSS;
        policy.next_wake_seconds = 10u;
        return policy;
    }

    if (input->telemetry_backlog >= 4u && input->battery_mwh >= 3800u) {
        policy.action = EHN_ACTION_TX;
        policy.next_wake_seconds = 5u;
        return policy;
    }

    if (input->seconds_since_sample >= 15u) {
        policy.action = EHN_ACTION_SAMPLE;
        policy.next_wake_seconds =
            (policy.mode == EHN_MODE_PERFORMANCE) ? 5u : 15u;
        return policy;
    }

    if (input->telemetry_backlog > 0u && input->harvested_mw >= 800u &&
        input->battery_mwh >= 4000u) {
        policy.action = EHN_ACTION_TX;
        policy.next_wake_seconds = 5u;
        return policy;
    }

    policy.action = EHN_ACTION_SLEEP;
    policy.next_wake_seconds = (policy.mode == EHN_MODE_PERFORMANCE) ? 8u : 20u;
    return policy;
}

