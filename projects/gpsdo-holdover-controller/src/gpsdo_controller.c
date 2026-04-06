#include "gpsdo_controller.h"

enum {
    GPSDO_BAD_PPS_THRESHOLD_NS = 5000,
    GPSDO_LOCK_TICKS_REQUIRED = 4
};

static uint16_t clamp_trim(int32_t trim_code) {
    if (trim_code < 0) {
        return 0u;
    }
    if (trim_code > 4095) {
        return 4095u;
    }
    return (uint16_t)trim_code;
}

void gpsdo_controller_init(gpsdo_controller_t *controller,
                           int16_t initial_temp_c_x10) {
    controller->state = GPSDO_STATE_ACQUIRE;
    controller->quality = GPSDO_QUALITY_SEARCH;
    controller->trim_code = 2048u;
    controller->lock_ticks = 0u;
    controller->holdover_seconds = 0u;
    controller->last_phase_ns = 0;
    drift_model_init(&controller->drift_model, initial_temp_c_x10);
}

gpsdo_output_t gpsdo_controller_step(gpsdo_controller_t *controller,
                                     const gpsdo_input_t *input) {
    gpsdo_output_t output;
    int32_t trim_delta;

    if (input->pps_valid &&
        (input->phase_error_ns > GPSDO_BAD_PPS_THRESHOLD_NS ||
         input->phase_error_ns < -GPSDO_BAD_PPS_THRESHOLD_NS)) {
        controller->state = GPSDO_STATE_FAULT;
        controller->quality = GPSDO_QUALITY_FAULT;
        controller->trim_code = 2048u;
        controller->holdover_seconds = 0u;
        controller->last_phase_ns = input->phase_error_ns;
    } else if (input->pps_valid) {
        controller->holdover_seconds = 0u;
        controller->quality = GPSDO_QUALITY_SEARCH;
        if (controller->state == GPSDO_STATE_HOLDOVER) {
            controller->lock_ticks = 1u;
        } else if (controller->lock_ticks < 255u) {
            controller->lock_ticks++;
        }

        trim_delta = -(input->phase_error_ns / 25);
        controller->trim_code = clamp_trim((int32_t)controller->trim_code + trim_delta);
        drift_model_update_locked(&controller->drift_model, (int16_t)trim_delta,
                                  input->temperature_c_x10);
        controller->last_phase_ns = input->phase_error_ns;

        if (controller->lock_ticks >= GPSDO_LOCK_TICKS_REQUIRED &&
            input->phase_error_ns < 80 && input->phase_error_ns > -80) {
            controller->state = GPSDO_STATE_TRACKING;
            controller->quality = GPSDO_QUALITY_LOCKED;
            controller->drift_model.anchor_temp_c_x10 = input->temperature_c_x10;
        } else {
            controller->state = GPSDO_STATE_ACQUIRE;
        }
    } else if (controller->state == GPSDO_STATE_TRACKING ||
               controller->state == GPSDO_STATE_ACQUIRE ||
               controller->state == GPSDO_STATE_HOLDOVER) {
        int16_t hold_trim;

        controller->state = GPSDO_STATE_HOLDOVER;
        controller->quality = GPSDO_QUALITY_HOLDOVER;
        controller->holdover_seconds++;
        hold_trim = drift_model_holdover_trim(&controller->drift_model,
                                              input->temperature_c_x10);
        controller->trim_code =
            clamp_trim((int32_t)controller->trim_code + hold_trim / 20);
        controller->last_phase_ns += hold_trim;
    }

    output.state = controller->state;
    output.quality = controller->quality;
    output.trim_code = controller->trim_code;
    output.estimated_phase_ns = controller->last_phase_ns;
    output.holdover_uncertainty_ns =
        (controller->state == GPSDO_STATE_HOLDOVER)
            ? drift_model_uncertainty(&controller->drift_model,
                                      input->temperature_c_x10,
                                      controller->holdover_seconds)
            : 0u;
    return output;
}
