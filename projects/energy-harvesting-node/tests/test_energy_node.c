#include <stdio.h>

#include "energy_budget.h"
#include "node_controller.h"
#include "task_policy.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static int test_brownout_forces_deep_sleep(void) {
    ehn_node_input_t input = {
        .battery_mwh = 1800u,
        .harvested_mw = 50u,
        .telemetry_backlog = 4u,
        .seconds_since_sample = 20u,
        .seconds_since_tx = 50u,
        .gnss_fix_required = true,
    };
    ehn_policy_t policy = ehn_policy_decide(&input);

    ASSERT_TRUE(policy.mode == EHN_MODE_SURVIVAL);
    ASSERT_TRUE(policy.action == EHN_ACTION_DEEP_SLEEP);
    ASSERT_TRUE(policy.next_wake_seconds == 60u);
    ASSERT_TRUE(policy.reserve_protected);
    return 0;
}

static int test_backlog_prefers_tx(void) {
    ehn_node_input_t input = {
        .battery_mwh = 4500u,
        .harvested_mw = 300u,
        .telemetry_backlog = 5u,
        .seconds_since_sample = 5u,
        .seconds_since_tx = 25u,
        .gnss_fix_required = false,
    };
    ehn_policy_t policy = ehn_policy_decide(&input);

    ASSERT_TRUE(policy.action == EHN_ACTION_TX);
    ASSERT_TRUE(policy.next_wake_seconds == 5u);
    return 0;
}

static int test_gnss_requires_energy_headroom(void) {
    ehn_node_input_t denied = {
        .battery_mwh = 5000u,
        .harvested_mw = 1200u,
        .telemetry_backlog = 0u,
        .seconds_since_sample = 8u,
        .seconds_since_tx = 10u,
        .gnss_fix_required = true,
    };
    ehn_node_input_t granted = denied;
    ehn_policy_t policy;

    policy = ehn_policy_decide(&denied);
    ASSERT_TRUE(policy.action != EHN_ACTION_GNSS);

    granted.battery_mwh = 6500u;
    policy = ehn_policy_decide(&granted);
    ASSERT_TRUE(policy.action == EHN_ACTION_GNSS);
    return 0;
}

static int test_sampling_increases_backlog(void) {
    ehn_node_state_t state;
    ehn_node_input_t input = {
        .battery_mwh = 5200u,
        .harvested_mw = 600u,
        .telemetry_backlog = 0u,
        .seconds_since_sample = 20u,
        .seconds_since_tx = 20u,
        .gnss_fix_required = false,
    };
    ehn_step_result_t result;

    ehn_node_init(&state, 5200u, 0u);
    result = ehn_node_step(&state, &input);
    ASSERT_TRUE(result.action == EHN_ACTION_SAMPLE);
    ASSERT_TRUE(result.telemetry_backlog == 1u);
    return 0;
}

static int test_energy_budget_charges_under_sleep(void) {
    uint16_t battery = ehn_energy_apply(5000u, 3600u, EHN_ACTION_SLEEP, 20u);

    ASSERT_TRUE(battery > 5000u);
    ASSERT_TRUE(battery < 5020u);
    return 0;
}

static int test_sleep_preserves_backlog(void) {
    ehn_node_state_t state;
    ehn_node_input_t input = {
        .battery_mwh = 5700u,
        .harvested_mw = 180u,
        .telemetry_backlog = 3u,
        .seconds_since_sample = 6u,
        .seconds_since_tx = 10u,
        .gnss_fix_required = false,
    };
    ehn_step_result_t result;

    ehn_node_init(&state, 5700u, 3u);
    result = ehn_node_step(&state, &input);
    ASSERT_TRUE(result.action == EHN_ACTION_SLEEP);
    ASSERT_TRUE(result.telemetry_backlog == 3u);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_brownout_forces_deep_sleep() == 0);
    ASSERT_TRUE(test_backlog_prefers_tx() == 0);
    ASSERT_TRUE(test_gnss_requires_energy_headroom() == 0);
    ASSERT_TRUE(test_sampling_increases_backlog() == 0);
    ASSERT_TRUE(test_energy_budget_charges_under_sleep() == 0);
    ASSERT_TRUE(test_sleep_preserves_backlog() == 0);
    printf("energy-harvesting-node tests passed\n");
    return 0;
}
