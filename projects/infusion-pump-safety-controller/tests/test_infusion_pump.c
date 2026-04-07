#include <assert.h>

#include "infusion_pump_controller.h"

static pump_input_t make_input(bool prime_request, bool start_request,
                               bool pause_request, bool resume_request,
                               bool reset_request, bool door_closed,
                               bool occlusion_detected, bool air_in_line,
                               unsigned reservoir_ml_tenths,
                               unsigned line_pressure_kpa) {
    pump_input_t input;

    input.prime_request = prime_request;
    input.start_request = start_request;
    input.pause_request = pause_request;
    input.resume_request = resume_request;
    input.reset_request = reset_request;
    input.door_closed = door_closed;
    input.occlusion_detected = occlusion_detected;
    input.air_in_line = air_in_line;
    input.reservoir_ml_tenths = reservoir_ml_tenths;
    input.line_pressure_kpa = line_pressure_kpa;
    return input;
}

static void test_idle_holds_pump(void) {
    infusion_pump_controller_t controller;
    pump_output_t output;
    pump_input_t input;

    infusion_pump_controller_init(&controller);
    input = make_input(false, false, false, false, false, true, false, false,
                       1200u, 18u);
    output = infusion_pump_controller_step(&controller, &input);

    assert(output.state == PUMP_STATE_IDLE);
    assert(output.command == PUMP_COMMAND_HOLD_PUMP);
    assert(output.delivered_ml_tenths == 0u);
}

static void test_prime_enters_priming(void) {
    infusion_pump_controller_t controller;
    pump_output_t output;
    pump_input_t input;

    infusion_pump_controller_init(&controller);
    input = make_input(true, false, false, false, false, true, false, false,
                       1190u, 24u);
    output = infusion_pump_controller_step(&controller, &input);

    assert(output.state == PUMP_STATE_PRIMING);
    assert(output.command == PUMP_COMMAND_PRIME_LINE);
    assert(output.motor_active);
}

static void test_infusion_completes_and_enters_kvo(void) {
    infusion_pump_controller_t controller;
    pump_output_t output;
    pump_input_t input;

    infusion_pump_controller_init(&controller);
    input = make_input(false, true, false, false, false, true, false, false,
                       1080u, 40u);
    output = infusion_pump_controller_step(&controller, &input);
    assert(output.state == PUMP_STATE_INFUSING);
    assert(output.delivered_ml_tenths == 120u);
    assert(output.therapy_remaining_ml_tenths == 120u);

    input = make_input(false, false, false, false, false, true, false, false,
                       960u, 36u);
    output = infusion_pump_controller_step(&controller, &input);
    assert(output.state == PUMP_STATE_KVO);
    assert(output.command == PUMP_COMMAND_KEEP_VEIN_OPEN);
    assert(output.delivered_ml_tenths == 240u);
    assert(output.therapy_remaining_ml_tenths == 0u);
}

static void test_pause_and_resume(void) {
    infusion_pump_controller_t controller;
    pump_output_t output;
    pump_input_t input;

    infusion_pump_controller_init(&controller);
    input = make_input(false, true, false, false, false, true, false, false,
                       1080u, 42u);
    (void)infusion_pump_controller_step(&controller, &input);

    input = make_input(false, false, true, false, false, true, false, false,
                       1080u, 28u);
    output = infusion_pump_controller_step(&controller, &input);
    assert(output.state == PUMP_STATE_PAUSED);
    assert(output.command == PUMP_COMMAND_PAUSE_INFUSION);

    input = make_input(false, false, false, true, false, true, false, false,
                       1080u, 30u);
    output = infusion_pump_controller_step(&controller, &input);
    assert(output.state == PUMP_STATE_INFUSING);
    assert(output.command == PUMP_COMMAND_RUN_INFUSION);
}

static void test_occlusion_trips_alarm(void) {
    infusion_pump_controller_t controller;
    pump_output_t output;
    pump_input_t input;

    infusion_pump_controller_init(&controller);
    input = make_input(false, true, false, false, false, true, false, false,
                       1080u, 40u);
    (void)infusion_pump_controller_step(&controller, &input);

    input = make_input(false, false, false, false, false, true, true, false,
                       1080u, 150u);
    output = infusion_pump_controller_step(&controller, &input);
    assert(output.state == PUMP_STATE_ALARM);
    assert(output.fault == PUMP_FAULT_OCCLUSION);
    assert(output.buzzer_active);
}

static void test_air_in_line_has_priority(void) {
    infusion_pump_controller_t controller;
    pump_output_t output;
    pump_input_t input;

    infusion_pump_controller_init(&controller);
    input = make_input(false, true, false, false, false, true, false, false,
                       1080u, 40u);
    (void)infusion_pump_controller_step(&controller, &input);

    input = make_input(false, false, false, false, false, true, true, true,
                       1080u, 150u);
    output = infusion_pump_controller_step(&controller, &input);
    assert(output.fault == PUMP_FAULT_AIR_IN_LINE);
}

static void test_reset_clears_alarm_when_safe(void) {
    infusion_pump_controller_t controller;
    pump_output_t output;
    pump_input_t input;

    infusion_pump_controller_init(&controller);
    input = make_input(false, true, false, false, false, true, false, false,
                       1080u, 40u);
    (void)infusion_pump_controller_step(&controller, &input);
    input = make_input(false, false, false, false, false, true, true, false,
                       1080u, 150u);
    (void)infusion_pump_controller_step(&controller, &input);

    input = make_input(false, false, false, false, true, true, false, false,
                       1080u, 20u);
    output = infusion_pump_controller_step(&controller, &input);
    assert(output.state == PUMP_STATE_IDLE);
    assert(output.command == PUMP_COMMAND_RESET_PUMP);
    assert(output.delivered_ml_tenths == 0u);
    assert(output.therapy_remaining_ml_tenths == 240u);
}

int main(void) {
    test_idle_holds_pump();
    test_prime_enters_priming();
    test_infusion_completes_and_enters_kvo();
    test_pause_and_resume();
    test_occlusion_trips_alarm();
    test_air_in_line_has_priority();
    test_reset_clears_alarm_when_safe();
    return 0;
}
