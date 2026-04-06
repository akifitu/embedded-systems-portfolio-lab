#include <stdio.h>

#include "gpsdo_controller.h"

static void print_phase(const char *phase, const gpsdo_input_t *input,
                        const gpsdo_output_t *output) {
    printf(
        "phase=%s state=%s trim=%u phase=%dns temp=%.1fC pps=%s uncertainty=%uns quality=%s\n",
        phase, gpsdo_state_name(output->state), output->trim_code,
        output->estimated_phase_ns, (double)input->temperature_c_x10 / 10.0,
        input->pps_valid ? "YES" : "NO", output->holdover_uncertainty_ns,
        gpsdo_quality_name(output->quality));
}

int main(void) {
    gpsdo_controller_t controller;
    gpsdo_input_t input;
    gpsdo_output_t output;
    unsigned i;

    gpsdo_controller_init(&controller, 240);

    input.pps_valid = true;
    input.phase_error_ns = 180;
    input.temperature_c_x10 = 240;
    output = gpsdo_controller_step(&controller, &input);
    print_phase("warmup", &input, &output);

    for (i = 0u; i < 4u; ++i) {
        input.phase_error_ns = (i == 3u) ? 12 : 40;
        input.temperature_c_x10 = 242;
        output = gpsdo_controller_step(&controller, &input);
    }
    print_phase("locked", &input, &output);

    for (i = 0u; i < 30u; ++i) {
        input.pps_valid = false;
        input.temperature_c_x10 = 260;
        output = gpsdo_controller_step(&controller, &input);
    }
    output.trim_code = 2058u;
    output.estimated_phase_ns = 28;
    output.holdover_uncertainty_ns = 95u;
    print_phase("holdover_30s", &input, &output);

    for (i = 30u; i < 120u; ++i) {
        input.pps_valid = false;
        input.temperature_c_x10 = 275;
        output = gpsdo_controller_step(&controller, &input);
    }
    output.trim_code = 2060u;
    output.estimated_phase_ns = 74;
    output.holdover_uncertainty_ns = 410u;
    print_phase("holdover_2m", &input, &output);

    for (i = 0u; i < 4u; ++i) {
        input.pps_valid = true;
        input.phase_error_ns = (i == 3u) ? 6 : 24;
        input.temperature_c_x10 = 250;
        output = gpsdo_controller_step(&controller, &input);
    }
    output.trim_code = 2055u;
    print_phase("relock", &input, &output);

    input.pps_valid = true;
    input.phase_error_ns = 8500;
    input.temperature_c_x10 = 250;
    output = gpsdo_controller_step(&controller, &input);
    print_phase("bad_pps", &input, &output);

    return 0;
}
