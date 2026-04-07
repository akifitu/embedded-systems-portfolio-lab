#include "lift_station_controller.h"

#include "level_policy.h"
#include "pump_arbiter.h"

static lift_output_t make_output(const lift_station_controller_t *controller,
                                 lift_command_t command, lift_fault_t fault) {
    lift_output_t output;

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.lead_pump = controller->preferred_lead;
    return output;
}

static lift_output_t enter_fault(lift_station_controller_t *controller,
                                 lift_fault_t fault) {
    controller->state = LIFT_STATE_FAULT;
    controller->latched_fault = fault;
    return make_output(controller, LIFT_COMMAND_STOP_ALL, fault);
}

static lift_command_t single_pump_command(lift_pump_t pump) {
    return (pump == LIFT_PUMP_A) ? LIFT_COMMAND_RUN_A : LIFT_COMMAND_RUN_B;
}

void lift_station_controller_init(lift_station_controller_t *controller) {
    controller->state = LIFT_STATE_IDLE;
    controller->latched_fault = LIFT_FAULT_NONE;
    controller->preferred_lead = LIFT_PUMP_A;
}

lift_output_t lift_station_controller_step(lift_station_controller_t *controller,
                                           const lift_input_t *input) {
    level_policy_t level;
    pump_selection_t selection;

    level = level_policy_evaluate(input->level_pct);
    selection = pump_arbiter_select(controller->preferred_lead, input->pump_a_ready,
                                    input->pump_b_ready, input->pump_a_seal_ok,
                                    input->pump_b_seal_ok);

    if (!input->pump_a_seal_ok && (controller->preferred_lead == LIFT_PUMP_A)) {
        return enter_fault(controller, LIFT_FAULT_PUMP_A_SEAL);
    }
    if (!input->pump_b_seal_ok && (controller->preferred_lead == LIFT_PUMP_B)) {
        return enter_fault(controller, LIFT_FAULT_PUMP_B_SEAL);
    }
    if (level.overflow) {
        return enter_fault(controller, LIFT_FAULT_OVERFLOW);
    }
    if (level.start_required && !selection.lead_available && !selection.lag_available) {
        return enter_fault(controller, LIFT_FAULT_NO_PUMP_AVAILABLE);
    }

    if (controller->state == LIFT_STATE_FAULT) {
        if (input->reset_request && level.stop_allowed && input->pump_a_seal_ok &&
            input->pump_b_seal_ok &&
            (input->pump_a_ready || input->pump_b_ready)) {
            controller->state = LIFT_STATE_IDLE;
            controller->latched_fault = LIFT_FAULT_NONE;
            return make_output(controller, LIFT_COMMAND_STOP_ALL, LIFT_FAULT_NONE);
        }
        return make_output(controller, LIFT_COMMAND_STOP_ALL,
                           controller->latched_fault);
    }

    if (!level.start_required) {
        if ((controller->state == LIFT_STATE_DRAIN_LEAD) ||
            (controller->state == LIFT_STATE_DRAIN_ASSIST)) {
            controller->preferred_lead =
                (controller->preferred_lead == LIFT_PUMP_A) ? LIFT_PUMP_B
                                                            : LIFT_PUMP_A;
        }
        controller->state = LIFT_STATE_IDLE;
        controller->latched_fault = LIFT_FAULT_NONE;
        return make_output(controller, LIFT_COMMAND_STOP_ALL, LIFT_FAULT_NONE);
    }

    controller->preferred_lead = selection.lead;

    if (level.assist_required && selection.lead_available && selection.lag_available) {
        controller->state = LIFT_STATE_DRAIN_ASSIST;
        controller->latched_fault = LIFT_FAULT_NONE;
        return make_output(controller, LIFT_COMMAND_RUN_BOTH, LIFT_FAULT_NONE);
    }

    if (selection.lead_available) {
        controller->state = LIFT_STATE_DRAIN_LEAD;
        controller->latched_fault = LIFT_FAULT_NONE;
        return make_output(controller, single_pump_command(selection.lead),
                           LIFT_FAULT_NONE);
    }

    return enter_fault(controller, LIFT_FAULT_NO_PUMP_AVAILABLE);
}
