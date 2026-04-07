#include <stdio.h>

#include "evse_controller.h"

static void print_phase(const char *label, const evse_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s pilot=%s limit=%u.%uA cool=%u contactor=%s "
        "fault=%s\n",
        label, evse_state_name(output->state), evse_command_name(output->command),
        evse_pilot_name(output->pilot_state), output->current_limit_a_x10 / 10u,
        output->current_limit_a_x10 % 10u, output->cooldown_remaining,
        output->contactor_closed_commanded ? "CLOSED" : "OPEN",
        evse_fault_name(output->fault));
}

static evse_input_t make_input(int16_t pilot_voltage_dv,
                               uint16_t pwm_duty_pct_x10,
                               uint16_t cable_limit_a_x10, int16_t temp_c_x10,
                               bool gfci_ok, bool contactor_closed_fb) {
    evse_input_t input;

    input.pilot_voltage_dv = pilot_voltage_dv;
    input.pwm_duty_pct_x10 = pwm_duty_pct_x10;
    input.cable_limit_a_x10 = cable_limit_a_x10;
    input.temp_c_x10 = temp_c_x10;
    input.gfci_ok = gfci_ok;
    input.contactor_closed_fb = contactor_closed_fb;
    return input;
}

static void demo_idle(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(120, 533u, 320u, 300, true, false);
    output = evse_controller_step(&controller, &input);
    print_phase("idle", &output);
}

static void demo_plugged(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(90, 533u, 320u, 300, true, false);
    output = evse_controller_step(&controller, &input);
    print_phase("plugged", &output);
}

static void demo_arming(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(60, 533u, 320u, 300, true, false);
    output = evse_controller_step(&controller, &input);
    print_phase("arming", &output);
}

static void demo_charging(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(60, 533u, 320u, 300, true, false);
    (void)evse_controller_step(&controller, &input);
    input.contactor_closed_fb = true;
    output = evse_controller_step(&controller, &input);
    print_phase("charging", &output);
}

static void demo_thermal_derate(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(60, 533u, 320u, 820, true, false);
    (void)evse_controller_step(&controller, &input);
    input.contactor_closed_fb = true;
    output = evse_controller_step(&controller, &input);
    print_phase("thermal_derate", &output);
}

static void demo_gfci_trip(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(60, 533u, 320u, 300, true, false);
    (void)evse_controller_step(&controller, &input);
    input.contactor_closed_fb = true;
    (void)evse_controller_step(&controller, &input);
    input.gfci_ok = false;
    output = evse_controller_step(&controller, &input);
    print_phase("gfci_trip", &output);
}

static void demo_cooldown(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(60, 533u, 320u, 300, true, false);
    (void)evse_controller_step(&controller, &input);
    input.contactor_closed_fb = true;
    (void)evse_controller_step(&controller, &input);
    input.gfci_ok = false;
    (void)evse_controller_step(&controller, &input);
    input = make_input(90, 533u, 320u, 300, true, false);
    output = evse_controller_step(&controller, &input);
    print_phase("cooldown", &output);
}

static void demo_recover(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(60, 533u, 320u, 300, true, false);
    (void)evse_controller_step(&controller, &input);
    input.contactor_closed_fb = true;
    (void)evse_controller_step(&controller, &input);
    input.gfci_ok = false;
    (void)evse_controller_step(&controller, &input);

    input = make_input(90, 533u, 320u, 300, true, false);
    (void)evse_controller_step(&controller, &input);
    (void)evse_controller_step(&controller, &input);
    (void)evse_controller_step(&controller, &input);

    input = make_input(60, 533u, 320u, 300, true, false);
    (void)evse_controller_step(&controller, &input);
    input.contactor_closed_fb = true;
    output = evse_controller_step(&controller, &input);
    print_phase("recover", &output);
}

int main(void) {
    demo_idle();
    demo_plugged();
    demo_arming();
    demo_charging();
    demo_thermal_derate();
    demo_gfci_trip();
    demo_cooldown();
    demo_recover();
    return 0;
}
