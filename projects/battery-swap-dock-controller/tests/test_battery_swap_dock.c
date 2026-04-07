#include <assert.h>

#include "battery_swap_dock_controller.h"

static swap_dock_input_t make_input(bool pack_present, bool enable_request,
                                    bool release_request, bool reset_request,
                                    bool aligned, bool auth_ok,
                                    unsigned pack_voltage_mv,
                                    unsigned bus_voltage_mv,
                                    unsigned dock_temp_c) {
    swap_dock_input_t input;

    input.pack_present = pack_present;
    input.enable_request = enable_request;
    input.release_request = release_request;
    input.reset_request = reset_request;
    input.aligned = aligned;
    input.auth_ok = auth_ok;
    input.pack_voltage_mv = pack_voltage_mv;
    input.bus_voltage_mv = bus_voltage_mv;
    input.dock_temp_c = dock_temp_c;
    return input;
}

static void test_idle_opens_empty_slot(void) {
    battery_swap_dock_controller_t controller;
    swap_dock_input_t input;
    swap_dock_output_t output;

    battery_swap_dock_controller_init(&controller);
    input = make_input(false, false, false, false, true, true, 0u, 0u, 28u);
    output = battery_swap_dock_controller_step(&controller, &input);

    assert(output.state == SWAP_DOCK_STATE_IDLE);
    assert(output.command == SWAP_DOCK_COMMAND_OPEN_DOCK);
    assert(!output.latch_closed);
}

static void test_validate_precharge_and_dock(void) {
    battery_swap_dock_controller_t controller;
    swap_dock_input_t input;
    swap_dock_output_t output;

    battery_swap_dock_controller_init(&controller);

    input = make_input(true, true, false, false, true, true, 13800u, 11600u,
                       31u);
    output = battery_swap_dock_controller_step(&controller, &input);
    assert(output.state == SWAP_DOCK_STATE_VALIDATE);
    assert(output.command == SWAP_DOCK_COMMAND_VERIFY_PACK);
    assert(output.delta_mv == 2200u);

    input = make_input(true, true, false, false, true, true, 13500u, 12600u,
                       33u);
    output = battery_swap_dock_controller_step(&controller, &input);
    assert(output.state == SWAP_DOCK_STATE_PRECHARGE);
    assert(output.command == SWAP_DOCK_COMMAND_PRECHARGE_BUS);
    assert(output.precharge_on);

    input = make_input(true, true, false, false, true, true, 13320u, 13200u,
                       34u);
    output = battery_swap_dock_controller_step(&controller, &input);
    assert(output.state == SWAP_DOCK_STATE_DOCKED);
    assert(output.command == SWAP_DOCK_COMMAND_CLOSE_MAIN_PATH);
    assert(output.main_closed);
}

static void test_release_opens_the_dock(void) {
    battery_swap_dock_controller_t controller;
    swap_dock_input_t input;
    swap_dock_output_t output;

    battery_swap_dock_controller_init(&controller);
    input = make_input(true, true, false, false, true, true, 13800u, 11600u,
                       31u);
    (void)battery_swap_dock_controller_step(&controller, &input);
    input = make_input(true, true, false, false, true, true, 13500u, 12600u,
                       33u);
    (void)battery_swap_dock_controller_step(&controller, &input);
    input = make_input(true, true, false, false, true, true, 13320u, 13200u,
                       34u);
    (void)battery_swap_dock_controller_step(&controller, &input);

    input = make_input(true, true, true, false, true, true, 13200u, 13200u,
                       30u);
    output = battery_swap_dock_controller_step(&controller, &input);

    assert(output.state == SWAP_DOCK_STATE_RELEASE);
    assert(output.command == SWAP_DOCK_COMMAND_RELEASE_PACK);
    assert(!output.latch_closed);
    assert(!output.main_closed);
}

static void test_auth_failure_latches_fault(void) {
    battery_swap_dock_controller_t controller;
    swap_dock_input_t input;
    swap_dock_output_t output;

    battery_swap_dock_controller_init(&controller);
    input = make_input(true, true, false, false, true, true, 13800u, 11600u,
                       31u);
    (void)battery_swap_dock_controller_step(&controller, &input);

    input = make_input(true, true, false, false, true, false, 13800u, 11600u,
                       31u);
    output = battery_swap_dock_controller_step(&controller, &input);

    assert(output.state == SWAP_DOCK_STATE_FAULT);
    assert(output.fault == SWAP_DOCK_FAULT_AUTH_FAIL);
}

static void test_precharge_timeout_latches_fault(void) {
    battery_swap_dock_controller_t controller;
    swap_dock_input_t input;
    swap_dock_output_t output;

    battery_swap_dock_controller_init(&controller);
    input = make_input(true, true, false, false, true, true, 13800u, 11600u,
                       31u);
    (void)battery_swap_dock_controller_step(&controller, &input);
    input = make_input(true, true, false, false, true, true, 13500u, 12600u,
                       33u);
    (void)battery_swap_dock_controller_step(&controller, &input);

    input = make_input(true, true, false, false, true, true, 13500u, 12600u,
                       33u);
    (void)battery_swap_dock_controller_step(&controller, &input);
    output = battery_swap_dock_controller_step(&controller, &input);
    output = battery_swap_dock_controller_step(&controller, &input);

    assert(output.state == SWAP_DOCK_STATE_FAULT);
    assert(output.fault == SWAP_DOCK_FAULT_PRECHARGE_TIMEOUT);
}

static void test_thermal_fault_and_safe_reset(void) {
    battery_swap_dock_controller_t controller;
    swap_dock_input_t input;
    swap_dock_output_t output;

    battery_swap_dock_controller_init(&controller);
    input = make_input(true, true, false, false, true, true, 13400u, 11800u,
                       34u);
    (void)battery_swap_dock_controller_step(&controller, &input);
    input = make_input(true, true, false, false, true, true, 13400u, 11800u,
                       82u);
    output = battery_swap_dock_controller_step(&controller, &input);
    assert(output.fault == SWAP_DOCK_FAULT_OVER_TEMP);
    assert(output.cooling_fan);

    input = make_input(true, false, false, true, true, true, 13400u, 11800u,
                       32u);
    output = battery_swap_dock_controller_step(&controller, &input);
    assert(output.state == SWAP_DOCK_STATE_FAULT);

    input = make_input(false, false, false, true, true, true, 0u, 0u, 32u);
    output = battery_swap_dock_controller_step(&controller, &input);
    assert(output.state == SWAP_DOCK_STATE_IDLE);
    assert(output.command == SWAP_DOCK_COMMAND_RESET_DOCK);
}

int main(void) {
    test_idle_opens_empty_slot();
    test_validate_precharge_and_dock();
    test_release_opens_the_dock();
    test_auth_failure_latches_fault();
    test_precharge_timeout_latches_fault();
    test_thermal_fault_and_safe_reset();
    return 0;
}
