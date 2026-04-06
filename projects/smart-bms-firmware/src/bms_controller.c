#include "bms_controller.h"

#include <stdbool.h>

#include "fault_manager.h"

static void update_balancing(const bms_sample_t *sample, bms_output_t *output) {
    float max_cell = sample->cell_voltage_v[0];
    float average = 0.0f;

    for (int i = 0; i < BMS_CELL_COUNT; ++i) {
        if (sample->cell_voltage_v[i] > max_cell) {
            max_cell = sample->cell_voltage_v[i];
        }
        average += sample->cell_voltage_v[i];
        output->balancing_enabled[i] = false;
    }

    average /= (float)BMS_CELL_COUNT;

    if ((max_cell - average) < 0.020f || max_cell < 4.05f) {
        return;
    }

    for (int i = 0; i < BMS_CELL_COUNT; ++i) {
        if ((sample->cell_voltage_v[i] > average + 0.015f) &&
            (max_cell - sample->cell_voltage_v[i] < 0.015f)) {
            output->balancing_enabled[i] = true;
        }
    }
}

void bms_controller_init(bms_controller_t *controller, float pack_capacity_ah,
                         float initial_soc_percent) {
    soc_estimator_init(&controller->soc, pack_capacity_ah, initial_soc_percent);
    controller->state = BMS_STATE_INIT;
}

bms_output_t bms_controller_step(bms_controller_t *controller,
                                 const bms_sample_t *sample) {
    bms_output_t output = {0};
    output.faults = fault_manager_evaluate(sample);
    output.soc_percent =
        soc_estimator_update(&controller->soc, sample->pack_current_a,
                             sample->dt_seconds);

    if (output.faults != BMS_FAULT_NONE) {
        output.state = BMS_STATE_FAULT;
        output.charge_enabled = false;
        output.discharge_enabled = false;
        controller->state = output.state;
        return output;
    }

    if (sample->pack_current_a < -1.0f) {
        output.state = BMS_STATE_CHARGING;
        output.charge_enabled = true;
        output.discharge_enabled = false;
    } else if (sample->pack_current_a > 1.0f) {
        output.state = BMS_STATE_DISCHARGING;
        output.charge_enabled = false;
        output.discharge_enabled = true;
    } else {
        output.state = BMS_STATE_IDLE;
        output.charge_enabled = true;
        output.discharge_enabled = true;
    }

    if (output.state == BMS_STATE_IDLE || output.state == BMS_STATE_CHARGING) {
        update_balancing(sample, &output);
    }

    controller->state = output.state;
    return output;
}

const char *bms_state_name(bms_state_t state) {
    switch (state) {
        case BMS_STATE_INIT:
            return "INIT";
        case BMS_STATE_IDLE:
            return "IDLE";
        case BMS_STATE_CHARGING:
            return "CHARGING";
        case BMS_STATE_DISCHARGING:
            return "DISCHARGING";
        case BMS_STATE_FAULT:
            return "FAULT";
        default:
            return "UNKNOWN";
    }
}

