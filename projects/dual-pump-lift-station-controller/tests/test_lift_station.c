#include <assert.h>

#include "lift_station_controller.h"

static void test_idle_holds_pumps_off(void) {
    lift_station_controller_t controller;
    lift_output_t output;

    lift_station_controller_init(&controller);
    output = lift_station_controller_step(
        &controller, &(lift_input_t){28u, 40u, true, true, true, true, false});

    assert(output.state == LIFT_STATE_IDLE);
    assert(output.command == LIFT_COMMAND_STOP_ALL);
    assert(output.fault == LIFT_FAULT_NONE);
    assert(output.lead_pump == LIFT_PUMP_A);
}

static void test_start_level_runs_lead_pump(void) {
    lift_station_controller_t controller;
    lift_output_t output;

    lift_station_controller_init(&controller);
    output = lift_station_controller_step(
        &controller, &(lift_input_t){72u, 120u, true, true, true, true, false});

    assert(output.state == LIFT_STATE_DRAIN_LEAD);
    assert(output.command == LIFT_COMMAND_RUN_A);
    assert(output.lead_pump == LIFT_PUMP_A);
}

static void test_assist_level_runs_both_pumps(void) {
    lift_station_controller_t controller;
    lift_output_t output;

    lift_station_controller_init(&controller);
    output = lift_station_controller_step(
        &controller, &(lift_input_t){91u, 180u, true, true, true, true, false});

    assert(output.state == LIFT_STATE_DRAIN_ASSIST);
    assert(output.command == LIFT_COMMAND_RUN_BOTH);
}

static void test_cycle_completion_swaps_preferred_lead(void) {
    lift_station_controller_t controller;
    lift_output_t output;

    lift_station_controller_init(&controller);
    (void)lift_station_controller_step(
        &controller, &(lift_input_t){72u, 120u, true, true, true, true, false});
    output = lift_station_controller_step(
        &controller, &(lift_input_t){30u, 35u, true, true, true, true, false});

    assert(output.state == LIFT_STATE_IDLE);
    assert(output.lead_pump == LIFT_PUMP_B);
}

static void test_backup_pump_runs_when_primary_unavailable(void) {
    lift_station_controller_t controller;
    lift_output_t output;

    lift_station_controller_init(&controller);
    (void)lift_station_controller_step(
        &controller, &(lift_input_t){72u, 120u, true, true, true, true, false});
    (void)lift_station_controller_step(
        &controller, &(lift_input_t){30u, 35u, true, true, true, true, false});
    output = lift_station_controller_step(
        &controller, &(lift_input_t){70u, 110u, false, true, true, true, false});

    assert(output.state == LIFT_STATE_DRAIN_LEAD);
    assert(output.command == LIFT_COMMAND_RUN_B);
    assert(output.lead_pump == LIFT_PUMP_B);
}

static void test_preferred_lead_seal_fault_locks_controller(void) {
    lift_station_controller_t controller;
    lift_output_t output;

    lift_station_controller_init(&controller);
    (void)lift_station_controller_step(
        &controller, &(lift_input_t){72u, 120u, true, true, true, true, false});
    (void)lift_station_controller_step(
        &controller, &(lift_input_t){30u, 35u, true, true, true, true, false});
    output = lift_station_controller_step(
        &controller, &(lift_input_t){68u, 100u, true, true, true, false, false});

    assert(output.state == LIFT_STATE_FAULT);
    assert(output.fault == LIFT_FAULT_PUMP_B_SEAL);
}

static void test_reset_recovers_fault_when_level_is_low(void) {
    lift_station_controller_t controller;
    lift_output_t output;

    lift_station_controller_init(&controller);
    (void)lift_station_controller_step(
        &controller, &(lift_input_t){72u, 120u, true, true, true, true, false});
    (void)lift_station_controller_step(
        &controller, &(lift_input_t){30u, 35u, true, true, true, true, false});
    (void)lift_station_controller_step(
        &controller, &(lift_input_t){68u, 100u, true, true, true, false, false});
    output = lift_station_controller_step(
        &controller, &(lift_input_t){24u, 20u, true, true, true, true, true});

    assert(output.state == LIFT_STATE_IDLE);
    assert(output.fault == LIFT_FAULT_NONE);
    assert(output.lead_pump == LIFT_PUMP_B);
}

int main(void) {
    test_idle_holds_pumps_off();
    test_start_level_runs_lead_pump();
    test_assist_level_runs_both_pumps();
    test_cycle_completion_swaps_preferred_lead();
    test_backup_pump_runs_when_primary_unavailable();
    test_preferred_lead_seal_fault_locks_controller();
    test_reset_recovers_fault_when_level_is_low();
    return 0;
}
