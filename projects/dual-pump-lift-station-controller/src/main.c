#include <stdio.h>

#include "lift_station_controller.h"

static const char *ready_label(bool pump_a_ready, bool pump_b_ready) {
    if (pump_a_ready && pump_b_ready) {
        return "AB";
    }
    if (pump_a_ready) {
        return "A";
    }
    if (pump_b_ready) {
        return "B";
    }
    return "NONE";
}

static void print_phase(const char *label, const lift_input_t *input,
                        const lift_output_t *output) {
    printf("phase=%s state=%s cmd=%s lead=%s level=%u inflow=%u ready=%s fault=%s\n",
           label, lift_state_name(output->state),
           lift_command_name(output->command), lift_pump_name(output->lead_pump),
           input->level_pct, input->inflow_lpm,
           ready_label(input->pump_a_ready, input->pump_b_ready),
           lift_fault_name(output->fault));
}

static lift_input_t make_input(uint8_t level_pct, uint16_t inflow_lpm,
                               bool pump_a_ready, bool pump_b_ready,
                               bool pump_a_seal_ok, bool pump_b_seal_ok,
                               bool reset_request) {
    lift_input_t input;

    input.level_pct = level_pct;
    input.inflow_lpm = inflow_lpm;
    input.pump_a_ready = pump_a_ready;
    input.pump_b_ready = pump_b_ready;
    input.pump_a_seal_ok = pump_a_seal_ok;
    input.pump_b_seal_ok = pump_b_seal_ok;
    input.reset_request = reset_request;
    return input;
}

static void demo_idle(void) {
    lift_station_controller_t controller;
    lift_input_t input;
    lift_output_t output;

    lift_station_controller_init(&controller);
    input = make_input(28u, 40u, true, true, true, true, false);
    output = lift_station_controller_step(&controller, &input);
    print_phase("idle", &input, &output);
}

static void demo_start_a(void) {
    lift_station_controller_t controller;
    lift_input_t input;
    lift_output_t output;

    lift_station_controller_init(&controller);
    input = make_input(72u, 120u, true, true, true, true, false);
    output = lift_station_controller_step(&controller, &input);
    print_phase("start_a", &input, &output);
}

static void demo_assist_b(void) {
    lift_station_controller_t controller;
    lift_input_t input;
    lift_output_t output;

    lift_station_controller_init(&controller);
    input = make_input(91u, 180u, true, true, true, true, false);
    output = lift_station_controller_step(&controller, &input);
    print_phase("assist_b", &input, &output);
}

static void demo_cycle_swap(void) {
    lift_station_controller_t controller;
    lift_input_t high_input;
    lift_input_t low_input;
    lift_output_t output;

    lift_station_controller_init(&controller);
    high_input = make_input(72u, 120u, true, true, true, true, false);
    (void)lift_station_controller_step(&controller, &high_input);
    low_input = make_input(30u, 35u, true, true, true, true, false);
    output = lift_station_controller_step(&controller, &low_input);
    print_phase("cycle_swap", &low_input, &output);
}

static void demo_single_backup(void) {
    lift_station_controller_t controller;
    lift_input_t high_input;
    lift_input_t low_input;
    lift_output_t output;

    lift_station_controller_init(&controller);
    high_input = make_input(72u, 120u, true, true, true, true, false);
    (void)lift_station_controller_step(&controller, &high_input);
    low_input = make_input(30u, 35u, true, true, true, true, false);
    (void)lift_station_controller_step(&controller, &low_input);
    high_input = make_input(70u, 110u, false, true, true, true, false);
    output = lift_station_controller_step(&controller, &high_input);
    print_phase("single_backup", &high_input, &output);
}

static void demo_seal_fault(void) {
    lift_station_controller_t controller;
    lift_input_t high_input;
    lift_input_t low_input;
    lift_input_t fault_input;
    lift_output_t output;

    lift_station_controller_init(&controller);
    high_input = make_input(72u, 120u, true, true, true, true, false);
    (void)lift_station_controller_step(&controller, &high_input);
    low_input = make_input(30u, 35u, true, true, true, true, false);
    (void)lift_station_controller_step(&controller, &low_input);
    fault_input = make_input(68u, 100u, true, true, true, false, false);
    output = lift_station_controller_step(&controller, &fault_input);
    print_phase("seal_fault", &fault_input, &output);
}

static void demo_recovered(void) {
    lift_station_controller_t controller;
    lift_input_t high_input;
    lift_input_t low_input;
    lift_input_t fault_input;
    lift_input_t recovery_input;
    lift_output_t output;

    lift_station_controller_init(&controller);
    high_input = make_input(72u, 120u, true, true, true, true, false);
    (void)lift_station_controller_step(&controller, &high_input);
    low_input = make_input(30u, 35u, true, true, true, true, false);
    (void)lift_station_controller_step(&controller, &low_input);
    fault_input = make_input(68u, 100u, true, true, true, false, false);
    (void)lift_station_controller_step(&controller, &fault_input);
    recovery_input = make_input(24u, 20u, true, true, true, true, true);
    output = lift_station_controller_step(&controller, &recovery_input);
    print_phase("recovered", &recovery_input, &output);
}

int main(void) {
    demo_idle();
    demo_start_a();
    demo_assist_b();
    demo_cycle_swap();
    demo_single_backup();
    demo_seal_fault();
    demo_recovered();
    return 0;
}
