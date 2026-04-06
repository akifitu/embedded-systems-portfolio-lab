#include <stdio.h>

#include "node_controller.h"

typedef struct {
    const char *label;
    ehn_node_input_t input;
} phase_t;

static void print_phase(const char *label, const ehn_step_result_t *result) {
    printf("phase=%s mode=%s action=%s battery=%umWh backlog=%u reserve=%s wake=%us\n",
           label, ehn_mode_name(result->mode), ehn_action_name(result->action),
           result->battery_mwh, result->telemetry_backlog,
           result->reserve_protected ? "YES" : "NO",
           result->next_wake_seconds);
}

int main(void) {
    ehn_node_state_t state;
    const phase_t phases[] = {
        {"dawn", {.battery_mwh = 5120u, .harvested_mw = 650u, .telemetry_backlog = 0u, .seconds_since_sample = 18u, .seconds_since_tx = 40u, .gnss_fix_required = false}},
        {"sunny", {.battery_mwh = 6120u, .harvested_mw = 1400u, .telemetry_backlog = 1u, .seconds_since_sample = 5u, .seconds_since_tx = 30u, .gnss_fix_required = false}},
        {"cloud", {.battery_mwh = 5700u, .harvested_mw = 180u, .telemetry_backlog = 3u, .seconds_since_sample = 6u, .seconds_since_tx = 10u, .gnss_fix_required = false}},
        {"brownout", {.battery_mwh = 1820u, .harvested_mw = 40u, .telemetry_backlog = 4u, .seconds_since_sample = 40u, .seconds_since_tx = 90u, .gnss_fix_required = true}},
        {"night-recovery", {.battery_mwh = 4430u, .harvested_mw = 900u, .telemetry_backlog = 4u, .seconds_since_sample = 12u, .seconds_since_tx = 70u, .gnss_fix_required = false}},
    };

    ehn_node_init(&state, 5120u, 0u);

    for (size_t i = 0; i < (sizeof(phases) / sizeof(phases[0])); ++i) {
        ehn_step_result_t result = ehn_node_step(&state, &phases[i].input);
        print_phase(phases[i].label, &result);
    }

    return 0;
}

