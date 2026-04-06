#include <assert.h>

#include "gpsdo_controller.h"

static void test_lock_acquisition_enters_tracking(void) {
    gpsdo_controller_t controller;
    gpsdo_input_t input;
    gpsdo_output_t output = {0};
    unsigned i;

    gpsdo_controller_init(&controller, 240);
    for (i = 0u; i < 5u; ++i) {
        input.pps_valid = true;
        input.phase_error_ns = (i == 4u) ? 12 : 40;
        input.temperature_c_x10 = 242;
        output = gpsdo_controller_step(&controller, &input);
    }

    assert(output.state == GPSDO_STATE_TRACKING);
    assert(output.quality == GPSDO_QUALITY_LOCKED);
}

static void test_holdover_uncertainty_grows(void) {
    gpsdo_controller_t controller;
    gpsdo_input_t input;
    gpsdo_output_t output = {0};
    unsigned i;
    uint32_t first_uncertainty;

    gpsdo_controller_init(&controller, 240);
    for (i = 0u; i < 5u; ++i) {
        input.pps_valid = true;
        input.phase_error_ns = (i == 4u) ? 12 : 40;
        input.temperature_c_x10 = 242;
        (void)gpsdo_controller_step(&controller, &input);
    }

    input.pps_valid = false;
    input.temperature_c_x10 = 255;
    output = gpsdo_controller_step(&controller, &input);
    first_uncertainty = output.holdover_uncertainty_ns;
    for (i = 0u; i < 20u; ++i) {
        output = gpsdo_controller_step(&controller, &input);
    }

    assert(output.state == GPSDO_STATE_HOLDOVER);
    assert(output.holdover_uncertainty_ns > first_uncertainty);
}

static void test_relock_resets_uncertainty(void) {
    gpsdo_controller_t controller;
    gpsdo_input_t input;
    gpsdo_output_t output = {0};
    unsigned i;

    gpsdo_controller_init(&controller, 240);
    for (i = 0u; i < 5u; ++i) {
        input.pps_valid = true;
        input.phase_error_ns = (i == 4u) ? 12 : 40;
        input.temperature_c_x10 = 242;
        (void)gpsdo_controller_step(&controller, &input);
    }
    input.pps_valid = false;
    input.temperature_c_x10 = 255;
    for (i = 0u; i < 20u; ++i) {
        (void)gpsdo_controller_step(&controller, &input);
    }
    for (i = 0u; i < 4u; ++i) {
        input.pps_valid = true;
        input.phase_error_ns = (i == 3u) ? 6 : 24;
        input.temperature_c_x10 = 250;
        output = gpsdo_controller_step(&controller, &input);
    }

    assert(output.state == GPSDO_STATE_TRACKING);
    assert(output.holdover_uncertainty_ns == 0u);
}

static void test_bad_pps_faults_controller(void) {
    gpsdo_controller_t controller;
    gpsdo_input_t input;
    gpsdo_output_t output;

    gpsdo_controller_init(&controller, 240);
    input.pps_valid = true;
    input.phase_error_ns = 8500;
    input.temperature_c_x10 = 250;
    output = gpsdo_controller_step(&controller, &input);

    assert(output.state == GPSDO_STATE_FAULT);
    assert(output.quality == GPSDO_QUALITY_FAULT);
}

int main(void) {
    test_lock_acquisition_enters_tracking();
    test_holdover_uncertainty_grows();
    test_relock_resets_uncertainty();
    test_bad_pps_faults_controller();
    return 0;
}
