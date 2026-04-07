#include <assert.h>

#include "evse_controller.h"

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

static void test_unplugged_stays_idle(void) {
    evse_controller_t controller;
    evse_output_t output;

    evse_controller_init(&controller);
    output = evse_controller_step(
        &controller, &(evse_input_t){120, 533u, 320u, 300, true, false});

    assert(output.state == EVSE_STATE_IDLE);
    assert(output.command == EVSE_COMMAND_OPEN_CONTACTOR);
    assert(output.current_limit_a_x10 == 0u);
}

static void test_connected_advertises_current(void) {
    evse_controller_t controller;
    evse_output_t output;

    evse_controller_init(&controller);
    output = evse_controller_step(
        &controller, &(evse_input_t){90, 533u, 320u, 300, true, false});

    assert(output.state == EVSE_STATE_CONNECTED);
    assert(output.command == EVSE_COMMAND_ADVERTISE_CURRENT);
    assert(output.current_limit_a_x10 == 320u);
}

static void test_vehicle_request_arms_then_charges(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(60, 533u, 320u, 300, true, false);
    output = evse_controller_step(&controller, &input);
    assert(output.state == EVSE_STATE_ARMING);
    assert(output.command == EVSE_COMMAND_CLOSE_CONTACTOR);

    input.contactor_closed_fb = true;
    output = evse_controller_step(&controller, &input);
    assert(output.state == EVSE_STATE_CHARGING);
    assert(output.command == EVSE_COMMAND_ENERGIZE_PORT);
}

static void test_thermal_derate_caps_current(void) {
    evse_controller_t controller;
    evse_output_t output;
    evse_input_t input;

    evse_controller_init(&controller);
    input = make_input(60, 533u, 320u, 820, true, false);
    (void)evse_controller_step(&controller, &input);
    input.contactor_closed_fb = true;
    output = evse_controller_step(&controller, &input);

    assert(output.state == EVSE_STATE_CHARGING);
    assert(output.current_limit_a_x10 == 180u);
}

static void test_gfci_fault_opens_port(void) {
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

    assert(output.state == EVSE_STATE_FAULT);
    assert(output.fault == EVSE_FAULT_GFCI);
    assert(output.current_limit_a_x10 == 0u);
}

static void test_cooldown_recovery_returns_to_charging(void) {
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
    assert(output.state == EVSE_STATE_COOLDOWN);
    assert(output.cooldown_remaining == 3u);

    output = evse_controller_step(&controller, &input);
    output = evse_controller_step(&controller, &input);
    output = evse_controller_step(&controller, &input);
    assert(output.state == EVSE_STATE_CONNECTED);

    input = make_input(60, 533u, 320u, 300, true, false);
    output = evse_controller_step(&controller, &input);
    assert(output.state == EVSE_STATE_ARMING);
    input.contactor_closed_fb = true;
    output = evse_controller_step(&controller, &input);
    assert(output.state == EVSE_STATE_CHARGING);
}

int main(void) {
    test_unplugged_stays_idle();
    test_connected_advertises_current();
    test_vehicle_request_arms_then_charges();
    test_thermal_derate_caps_current();
    test_gfci_fault_opens_port();
    test_cooldown_recovery_returns_to_charging();
    return 0;
}
