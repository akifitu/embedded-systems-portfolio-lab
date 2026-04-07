#include "evse_controller.h"

#include "current_policy.h"
#include "pilot_decoder.h"

#define EVSE_OVERTEMP_C_X10 900
#define EVSE_COOLDOWN_FRAMES 3u

static evse_output_t make_output(const evse_controller_t *controller,
                                 const pilot_decode_t *pilot,
                                 evse_command_t command, evse_fault_t fault,
                                 uint16_t current_limit_a_x10,
                                 bool contactor_closed_commanded) {
    evse_output_t output;

    output.state = controller->state;
    output.command = command;
    output.pilot_state = pilot->pilot_state;
    output.fault = fault;
    output.current_limit_a_x10 = current_limit_a_x10;
    output.cooldown_remaining = controller->cooldown_remaining;
    output.contactor_closed_commanded = contactor_closed_commanded;
    return output;
}

static evse_output_t enter_fault(evse_controller_t *controller,
                                 const pilot_decode_t *pilot,
                                 evse_fault_t fault) {
    controller->state = EVSE_STATE_FAULT;
    controller->latched_fault = fault;
    controller->cooldown_remaining = EVSE_COOLDOWN_FRAMES;
    return make_output(controller, pilot, EVSE_COMMAND_OPEN_CONTACTOR, fault, 0u,
                       false);
}

void evse_controller_init(evse_controller_t *controller) {
    controller->state = EVSE_STATE_IDLE;
    controller->cooldown_remaining = 0u;
    controller->latched_fault = EVSE_FAULT_NONE;
}

evse_output_t evse_controller_step(evse_controller_t *controller,
                                   const evse_input_t *input) {
    pilot_decode_t pilot;
    uint16_t current_limit_a_x10;

    pilot = pilot_decoder_decode(input->pilot_voltage_dv, input->pwm_duty_pct_x10);
    current_limit_a_x10 = current_policy_limit_a_x10(
        pilot.advertised_current_a_x10, input->cable_limit_a_x10,
        input->temp_c_x10);

    if (!input->gfci_ok) {
        return enter_fault(controller, &pilot, EVSE_FAULT_GFCI);
    }

    if (input->temp_c_x10 >= EVSE_OVERTEMP_C_X10) {
        return enter_fault(controller, &pilot, EVSE_FAULT_OVERTEMP);
    }

    if (pilot.pilot_state == EVSE_PILOT_F) {
        return enter_fault(controller, &pilot, EVSE_FAULT_PILOT);
    }

    if (((controller->state == EVSE_STATE_IDLE) ||
         (controller->state == EVSE_STATE_CONNECTED) ||
         (controller->state == EVSE_STATE_COOLDOWN) ||
         (controller->state == EVSE_STATE_FAULT)) &&
        input->contactor_closed_fb) {
        return enter_fault(controller, &pilot, EVSE_FAULT_CONTACTOR);
    }

    if ((controller->state == EVSE_STATE_ARMING) &&
        (pilot.pilot_state != EVSE_PILOT_C)) {
        controller->state = EVSE_STATE_CONNECTED;
    }

    if ((controller->state == EVSE_STATE_CHARGING) &&
        !input->contactor_closed_fb) {
        return enter_fault(controller, &pilot, EVSE_FAULT_CONTACTOR);
    }

    if (controller->state == EVSE_STATE_FAULT) {
        controller->state = EVSE_STATE_COOLDOWN;
        return make_output(controller, &pilot, EVSE_COMMAND_HOLD_COOLDOWN,
                           controller->latched_fault, 0u, false);
    }

    if (controller->state == EVSE_STATE_COOLDOWN) {
        if (controller->cooldown_remaining > 0u) {
            controller->cooldown_remaining--;
        }
        if (controller->cooldown_remaining > 0u) {
            return make_output(controller, &pilot, EVSE_COMMAND_HOLD_COOLDOWN,
                               controller->latched_fault, 0u, false);
        }

        controller->state = EVSE_STATE_IDLE;
        controller->latched_fault = EVSE_FAULT_NONE;
    }

    switch (pilot.pilot_state) {
        case EVSE_PILOT_A:
            controller->state = EVSE_STATE_IDLE;
            controller->latched_fault = EVSE_FAULT_NONE;
            return make_output(controller, &pilot, EVSE_COMMAND_OPEN_CONTACTOR,
                               EVSE_FAULT_NONE, 0u, false);

        case EVSE_PILOT_B:
            controller->state = EVSE_STATE_CONNECTED;
            controller->latched_fault = EVSE_FAULT_NONE;
            return make_output(controller, &pilot, EVSE_COMMAND_ADVERTISE_CURRENT,
                               EVSE_FAULT_NONE, current_limit_a_x10, false);

        case EVSE_PILOT_C:
            if (input->contactor_closed_fb) {
                controller->state = EVSE_STATE_CHARGING;
                controller->latched_fault = EVSE_FAULT_NONE;
                return make_output(controller, &pilot, EVSE_COMMAND_ENERGIZE_PORT,
                                   EVSE_FAULT_NONE, current_limit_a_x10, true);
            }

            controller->state = EVSE_STATE_ARMING;
            controller->latched_fault = EVSE_FAULT_NONE;
            return make_output(controller, &pilot, EVSE_COMMAND_CLOSE_CONTACTOR,
                               EVSE_FAULT_NONE, current_limit_a_x10, false);

        case EVSE_PILOT_F:
        default:
            return enter_fault(controller, &pilot, EVSE_FAULT_PILOT);
    }
}
