#include "infusion_pump_controller.h"

#include "dose_engine.h"
#include "safety_monitor.h"

#define PROGRAMMED_DOSE_TENTHS 240u
#define INFUSION_STEP_TENTHS 120u
#define INFUSION_RATE_MLPH 120u
#define PRIME_RATE_MLPH 300u
#define KVO_RATE_MLPH 5u

static pump_output_t make_output(const infusion_pump_controller_t *controller,
                                 const pump_input_t *input,
                                 pump_command_t command,
                                 pump_fault_t fault) {
    pump_output_t output;

    output.state = controller->state;
    output.command = command;
    output.fault = fault;
    output.motor_active = false;
    output.clamp_open = false;
    output.buzzer_active = false;
    output.rate_mlph = 0u;
    output.delivered_ml_tenths = controller->delivered_ml_tenths;
    output.therapy_remaining_ml_tenths = controller->therapy_remaining_ml_tenths;
    output.reservoir_ml_tenths = input->reservoir_ml_tenths;
    output.line_pressure_kpa = input->line_pressure_kpa;

    switch (controller->state) {
        case PUMP_STATE_PRIMING:
            output.motor_active = true;
            output.clamp_open = true;
            output.rate_mlph = PRIME_RATE_MLPH;
            break;

        case PUMP_STATE_INFUSING:
            output.motor_active = true;
            output.clamp_open = true;
            output.rate_mlph = INFUSION_RATE_MLPH;
            break;

        case PUMP_STATE_KVO:
            output.motor_active = true;
            output.clamp_open = true;
            output.rate_mlph = KVO_RATE_MLPH;
            break;

        case PUMP_STATE_ALARM:
            output.buzzer_active = true;
            break;

        case PUMP_STATE_PAUSED:
        case PUMP_STATE_IDLE:
        default:
            break;
    }

    return output;
}

static pump_output_t enter_alarm(infusion_pump_controller_t *controller,
                                 const pump_input_t *input,
                                 pump_fault_t fault) {
    controller->state = PUMP_STATE_ALARM;
    controller->latched_fault = fault;
    return make_output(controller, input, PUMP_COMMAND_STOP_AND_ALARM, fault);
}

void infusion_pump_controller_init(infusion_pump_controller_t *controller) {
    controller->state = PUMP_STATE_IDLE;
    controller->latched_fault = PUMP_FAULT_NONE;
    controller->delivered_ml_tenths = 0u;
    controller->therapy_remaining_ml_tenths = PROGRAMMED_DOSE_TENTHS;
}

pump_output_t infusion_pump_controller_step(infusion_pump_controller_t *controller,
                                            const pump_input_t *input) {
    pump_fault_t active_fault;
    dose_engine_result_t result;

    active_fault = safety_monitor_fault(input);

    if (controller->state == PUMP_STATE_ALARM) {
        if (input->reset_request && (active_fault == PUMP_FAULT_NONE)) {
            controller->state = PUMP_STATE_IDLE;
            controller->latched_fault = PUMP_FAULT_NONE;
            controller->delivered_ml_tenths = 0u;
            controller->therapy_remaining_ml_tenths = PROGRAMMED_DOSE_TENTHS;
            return make_output(controller, input, PUMP_COMMAND_RESET_PUMP,
                               PUMP_FAULT_NONE);
        }

        return make_output(controller, input, PUMP_COMMAND_STOP_AND_ALARM,
                           controller->latched_fault);
    }

    if (active_fault != PUMP_FAULT_NONE) {
        return enter_alarm(controller, input, active_fault);
    }

    switch (controller->state) {
        case PUMP_STATE_IDLE:
            if (input->prime_request) {
                controller->state = PUMP_STATE_PRIMING;
                return make_output(controller, input, PUMP_COMMAND_PRIME_LINE,
                                   PUMP_FAULT_NONE);
            }
            if (input->start_request) {
                controller->state = PUMP_STATE_INFUSING;
                result = dose_engine_advance(controller->therapy_remaining_ml_tenths,
                                             INFUSION_STEP_TENTHS);
                controller->delivered_ml_tenths += result.delivered_step_tenths;
                controller->therapy_remaining_ml_tenths =
                    result.therapy_remaining_tenths;
                if (result.therapy_complete) {
                    controller->state = PUMP_STATE_KVO;
                    return make_output(controller, input,
                                       PUMP_COMMAND_KEEP_VEIN_OPEN,
                                       PUMP_FAULT_NONE);
                }
                return make_output(controller, input,
                                   PUMP_COMMAND_RUN_INFUSION, PUMP_FAULT_NONE);
            }
            return make_output(controller, input, PUMP_COMMAND_HOLD_PUMP,
                               PUMP_FAULT_NONE);

        case PUMP_STATE_PRIMING:
            if (input->start_request) {
                controller->state = PUMP_STATE_INFUSING;
                result = dose_engine_advance(controller->therapy_remaining_ml_tenths,
                                             INFUSION_STEP_TENTHS);
                controller->delivered_ml_tenths += result.delivered_step_tenths;
                controller->therapy_remaining_ml_tenths =
                    result.therapy_remaining_tenths;
                if (result.therapy_complete) {
                    controller->state = PUMP_STATE_KVO;
                    return make_output(controller, input,
                                       PUMP_COMMAND_KEEP_VEIN_OPEN,
                                       PUMP_FAULT_NONE);
                }
                return make_output(controller, input,
                                   PUMP_COMMAND_RUN_INFUSION, PUMP_FAULT_NONE);
            }
            if (!input->prime_request) {
                controller->state = PUMP_STATE_IDLE;
                return make_output(controller, input, PUMP_COMMAND_HOLD_PUMP,
                                   PUMP_FAULT_NONE);
            }
            return make_output(controller, input, PUMP_COMMAND_PRIME_LINE,
                               PUMP_FAULT_NONE);

        case PUMP_STATE_INFUSING:
            if (input->pause_request) {
                controller->state = PUMP_STATE_PAUSED;
                return make_output(controller, input,
                                   PUMP_COMMAND_PAUSE_INFUSION,
                                   PUMP_FAULT_NONE);
            }

            result = dose_engine_advance(controller->therapy_remaining_ml_tenths,
                                         INFUSION_STEP_TENTHS);
            controller->delivered_ml_tenths += result.delivered_step_tenths;
            controller->therapy_remaining_ml_tenths =
                result.therapy_remaining_tenths;

            if (result.therapy_complete) {
                controller->state = PUMP_STATE_KVO;
                return make_output(controller, input,
                                   PUMP_COMMAND_KEEP_VEIN_OPEN,
                                   PUMP_FAULT_NONE);
            }

            return make_output(controller, input, PUMP_COMMAND_RUN_INFUSION,
                               PUMP_FAULT_NONE);

        case PUMP_STATE_PAUSED:
            if (input->resume_request) {
                controller->state = PUMP_STATE_INFUSING;
                return make_output(controller, input,
                                   PUMP_COMMAND_RUN_INFUSION,
                                   PUMP_FAULT_NONE);
            }
            return make_output(controller, input, PUMP_COMMAND_PAUSE_INFUSION,
                               PUMP_FAULT_NONE);

        case PUMP_STATE_KVO:
            return make_output(controller, input, PUMP_COMMAND_KEEP_VEIN_OPEN,
                               PUMP_FAULT_NONE);

        case PUMP_STATE_ALARM:
        default:
            return make_output(controller, input, PUMP_COMMAND_STOP_AND_ALARM,
                               controller->latched_fault);
    }
}
