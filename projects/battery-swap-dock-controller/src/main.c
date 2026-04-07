#include <stdio.h>

#include "battery_swap_dock_controller.h"

static const char *open_closed(bool value) {
    return value ? "CLOSED" : "OPEN";
}

static const char *on_off(bool value) {
    return value ? "ON" : "OFF";
}

static const char *present_absent(bool value) {
    return value ? "PRESENT" : "ABSENT";
}

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

static void print_phase(const char *phase, const swap_dock_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s fault=%s pack=%s latch=%s main=%s "
        "precharge=%s fan=%s delta=%umV progress=%u\n",
        phase, swap_dock_state_name(output->state),
        swap_dock_command_name(output->command),
        swap_dock_fault_name(output->fault),
        present_absent(output->pack_present), open_closed(output->latch_closed),
        open_closed(output->main_closed), on_off(output->precharge_on),
        on_off(output->cooling_fan), output->delta_mv, output->progress_pct);
}

int main(void) {
    battery_swap_dock_controller_t controller;
    battery_swap_dock_controller_t fault_controller;
    swap_dock_input_t input;
    swap_dock_output_t output;

    battery_swap_dock_controller_init(&controller);
    input = make_input(false, false, false, false, true, true, 0u, 0u, 28u);
    output = battery_swap_dock_controller_step(&controller, &input);
    print_phase("slot_idle", &output);

    input = make_input(true, true, false, false, true, true, 13800u, 11600u,
                       31u);
    output = battery_swap_dock_controller_step(&controller, &input);
    print_phase("validate_pack", &output);

    input = make_input(true, true, false, false, true, true, 13500u, 12600u,
                       33u);
    output = battery_swap_dock_controller_step(&controller, &input);
    print_phase("precharge_ramp", &output);

    input = make_input(true, true, false, false, true, true, 13320u, 13200u,
                       34u);
    output = battery_swap_dock_controller_step(&controller, &input);
    print_phase("docked_ready", &output);

    input = make_input(true, true, true, false, true, true, 13200u, 13200u,
                       30u);
    output = battery_swap_dock_controller_step(&controller, &input);
    print_phase("release_cycle", &output);

    battery_swap_dock_controller_init(&fault_controller);
    input = make_input(true, true, false, false, true, true, 13400u, 11800u,
                       34u);
    (void)battery_swap_dock_controller_step(&fault_controller, &input);
    input = make_input(true, true, false, false, true, true, 13400u, 11800u,
                       82u);
    output = battery_swap_dock_controller_step(&fault_controller, &input);
    print_phase("thermal_fault", &output);

    input = make_input(false, false, false, true, true, true, 0u, 0u, 32u);
    output = battery_swap_dock_controller_step(&fault_controller, &input);
    print_phase("reset_ready", &output);

    return 0;
}
