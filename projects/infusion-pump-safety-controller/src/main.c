#include <stdio.h>

#include "infusion_pump_controller.h"

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

static const char *open_closed(bool value) {
    return value ? "OPEN" : "CLOSED";
}

static double tenths_to_ml(unsigned value) {
    return (double)value / 10.0;
}

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

static void print_phase(const char *phase, const pump_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s motor=%s clamp=%s buzzer=%s "
        "rate=%umLph delivered=%.1fmL remaining=%.1fmL reservoir=%.1fmL "
        "pressure=%ukPa\n",
        phase, pump_state_name(output->state),
        pump_command_name(output->command), pump_fault_name(output->fault),
        on_off(output->motor_active), open_closed(output->clamp_open),
        on_off(output->buzzer_active), output->rate_mlph,
        tenths_to_ml(output->delivered_ml_tenths),
        tenths_to_ml(output->therapy_remaining_ml_tenths),
        tenths_to_ml(output->reservoir_ml_tenths), output->line_pressure_kpa);
}

int main(void) {
    infusion_pump_controller_t therapy_controller;
    infusion_pump_controller_t alarm_controller;
    pump_output_t output;
    pump_input_t input;

    infusion_pump_controller_init(&therapy_controller);
    input = make_input(false, false, false, false, false, true, false, false,
                       1200u, 18u);
    output = infusion_pump_controller_step(&therapy_controller, &input);
    print_phase("idle", &output);

    input = make_input(true, false, false, false, false, true, false, false,
                       1185u, 24u);
    output = infusion_pump_controller_step(&therapy_controller, &input);
    print_phase("priming", &output);

    input = make_input(false, true, false, false, false, true, false, false,
                       1065u, 42u);
    output = infusion_pump_controller_step(&therapy_controller, &input);
    print_phase("infusing", &output);

    input = make_input(false, false, true, false, false, true, false, false,
                       1065u, 28u);
    output = infusion_pump_controller_step(&therapy_controller, &input);
    print_phase("paused", &output);

    input = make_input(false, false, false, true, false, true, false, false,
                       945u, 38u);
    (void)infusion_pump_controller_step(&therapy_controller, &input);
    output = infusion_pump_controller_step(&therapy_controller, &input);
    print_phase("kvo", &output);

    infusion_pump_controller_init(&alarm_controller);
    input = make_input(false, true, false, false, false, true, false, false,
                       1065u, 36u);
    (void)infusion_pump_controller_step(&alarm_controller, &input);
    input = make_input(false, false, false, false, false, true, true, false,
                       1065u, 145u);
    output = infusion_pump_controller_step(&alarm_controller, &input);
    print_phase("occlusion_alarm", &output);

    input = make_input(false, false, false, false, true, true, false, false,
                       1065u, 20u);
    output = infusion_pump_controller_step(&alarm_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
