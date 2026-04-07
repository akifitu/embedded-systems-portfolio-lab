#include "battery_swap_dock_controller.h"

#include "dock_policy.h"
#include "precharge_monitor.h"

#define DOCK_OVERTEMP_C 75u
#define PRECHARGE_TIMEOUT_FRAMES 2u

static swap_dock_fault_t active_fault(
    const battery_swap_dock_controller_t *controller,
    const swap_dock_input_t *input,
    unsigned delta_mv) {
    if (input->dock_temp_c >= DOCK_OVERTEMP_C) {
        return SWAP_DOCK_FAULT_OVER_TEMP;
    }

    if (!input->pack_present) {
        return SWAP_DOCK_FAULT_NONE;
    }

    if ((controller->state != SWAP_DOCK_STATE_IDLE) && !input->aligned) {
        return SWAP_DOCK_FAULT_MISALIGNED;
    }

    if ((controller->state != SWAP_DOCK_STATE_IDLE) && !input->auth_ok) {
        return SWAP_DOCK_FAULT_AUTH_FAIL;
    }

    if (((controller->state == SWAP_DOCK_STATE_VALIDATE) ||
         (controller->state == SWAP_DOCK_STATE_PRECHARGE)) &&
        precharge_voltage_mismatch(delta_mv)) {
        return SWAP_DOCK_FAULT_VOLTAGE_MISMATCH;
    }

    return SWAP_DOCK_FAULT_NONE;
}

static swap_dock_output_t make_output(
    const battery_swap_dock_controller_t *controller,
    const swap_dock_input_t *input,
    swap_dock_command_t command,
    swap_dock_fault_t fault) {
    swap_dock_output_t output;
    unsigned delta_mv;
    dock_policy_output_t policy;

    delta_mv = precharge_delta_mv(input);
    policy = dock_policy_apply(controller->state, input, fault, delta_mv);

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.pack_present = input->pack_present;
    output.latch_closed = policy.latch_closed;
    output.main_closed = policy.main_closed;
    output.precharge_on = policy.precharge_on;
    output.cooling_fan = policy.cooling_fan;
    output.delta_mv = delta_mv;
    output.progress_pct = policy.progress_pct;
    output.pack_voltage_mv = input->pack_voltage_mv;
    output.bus_voltage_mv = input->bus_voltage_mv;
    output.dock_temp_c = input->dock_temp_c;
    return output;
}

static swap_dock_output_t enter_fault(
    battery_swap_dock_controller_t *controller,
    const swap_dock_input_t *input,
    swap_dock_fault_t fault) {
    controller->state = SWAP_DOCK_STATE_FAULT;
    controller->latched_fault = fault;
    controller->precharge_frames = 0u;
    return make_output(controller, input, SWAP_DOCK_COMMAND_LATCH_FAULT, fault);
}

void battery_swap_dock_controller_init(
    battery_swap_dock_controller_t *controller) {
    controller->state = SWAP_DOCK_STATE_IDLE;
    controller->latched_fault = SWAP_DOCK_FAULT_NONE;
    controller->precharge_frames = 0u;
}

swap_dock_output_t battery_swap_dock_controller_step(
    battery_swap_dock_controller_t *controller,
    const swap_dock_input_t *input) {
    unsigned delta_mv;
    swap_dock_fault_t fault;

    delta_mv = precharge_delta_mv(input);

    if (controller->state == SWAP_DOCK_STATE_FAULT) {
        if (input->reset_request && !input->pack_present &&
            (input->dock_temp_c < DOCK_OVERTEMP_C)) {
            controller->state = SWAP_DOCK_STATE_IDLE;
            controller->latched_fault = SWAP_DOCK_FAULT_NONE;
            controller->precharge_frames = 0u;
            return make_output(controller, input, SWAP_DOCK_COMMAND_RESET_DOCK,
                               SWAP_DOCK_FAULT_NONE);
        }

        return make_output(controller, input, SWAP_DOCK_COMMAND_LATCH_FAULT,
                           controller->latched_fault);
    }

    fault = active_fault(controller, input, delta_mv);
    if (fault != SWAP_DOCK_FAULT_NONE) {
        return enter_fault(controller, input, fault);
    }

    if (!input->pack_present) {
        controller->state = SWAP_DOCK_STATE_IDLE;
        controller->precharge_frames = 0u;
        return make_output(controller, input, SWAP_DOCK_COMMAND_OPEN_DOCK,
                           SWAP_DOCK_FAULT_NONE);
    }

    switch (controller->state) {
        case SWAP_DOCK_STATE_IDLE:
            if (!input->enable_request) {
                return make_output(controller, input, SWAP_DOCK_COMMAND_OPEN_DOCK,
                                   SWAP_DOCK_FAULT_NONE);
            }

            controller->state = SWAP_DOCK_STATE_VALIDATE;
            return make_output(controller, input, SWAP_DOCK_COMMAND_VERIFY_PACK,
                               SWAP_DOCK_FAULT_NONE);

        case SWAP_DOCK_STATE_VALIDATE:
            if (input->release_request) {
                controller->state = SWAP_DOCK_STATE_RELEASE;
                return make_output(controller, input,
                                   SWAP_DOCK_COMMAND_RELEASE_PACK,
                                   SWAP_DOCK_FAULT_NONE);
            }

            controller->state = SWAP_DOCK_STATE_PRECHARGE;
            controller->precharge_frames = 0u;
            return make_output(controller, input,
                               SWAP_DOCK_COMMAND_PRECHARGE_BUS,
                               SWAP_DOCK_FAULT_NONE);

        case SWAP_DOCK_STATE_PRECHARGE:
            if (input->release_request) {
                controller->state = SWAP_DOCK_STATE_RELEASE;
                controller->precharge_frames = 0u;
                return make_output(controller, input,
                                   SWAP_DOCK_COMMAND_RELEASE_PACK,
                                   SWAP_DOCK_FAULT_NONE);
            }

            if (precharge_ready(delta_mv)) {
                controller->state = SWAP_DOCK_STATE_DOCKED;
                controller->precharge_frames = 0u;
                return make_output(controller, input,
                                   SWAP_DOCK_COMMAND_CLOSE_MAIN_PATH,
                                   SWAP_DOCK_FAULT_NONE);
            }

            if (controller->precharge_frames >= PRECHARGE_TIMEOUT_FRAMES) {
                return enter_fault(controller, input,
                                   SWAP_DOCK_FAULT_PRECHARGE_TIMEOUT);
            }

            controller->precharge_frames++;
            return make_output(controller, input,
                               SWAP_DOCK_COMMAND_PRECHARGE_BUS,
                               SWAP_DOCK_FAULT_NONE);

        case SWAP_DOCK_STATE_DOCKED:
            if (input->release_request) {
                controller->state = SWAP_DOCK_STATE_RELEASE;
                return make_output(controller, input,
                                   SWAP_DOCK_COMMAND_RELEASE_PACK,
                                   SWAP_DOCK_FAULT_NONE);
            }

            return make_output(controller, input,
                               SWAP_DOCK_COMMAND_CLOSE_MAIN_PATH,
                               SWAP_DOCK_FAULT_NONE);

        case SWAP_DOCK_STATE_RELEASE:
            if (!input->release_request) {
                controller->state = SWAP_DOCK_STATE_IDLE;
                return make_output(controller, input,
                                   SWAP_DOCK_COMMAND_OPEN_DOCK,
                                   SWAP_DOCK_FAULT_NONE);
            }

            return make_output(controller, input,
                               SWAP_DOCK_COMMAND_RELEASE_PACK,
                               SWAP_DOCK_FAULT_NONE);

        case SWAP_DOCK_STATE_FAULT:
        default:
            return make_output(controller, input, SWAP_DOCK_COMMAND_LATCH_FAULT,
                               controller->latched_fault);
    }
}
