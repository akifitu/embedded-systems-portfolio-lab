#include "fault_manager.h"

static float cell_max(const bms_sample_t *sample) {
    float max_value = sample->cell_voltage_v[0];
    for (int i = 1; i < BMS_CELL_COUNT; ++i) {
        if (sample->cell_voltage_v[i] > max_value) {
            max_value = sample->cell_voltage_v[i];
        }
    }
    return max_value;
}

static float cell_min(const bms_sample_t *sample) {
    float min_value = sample->cell_voltage_v[0];
    for (int i = 1; i < BMS_CELL_COUNT; ++i) {
        if (sample->cell_voltage_v[i] < min_value) {
            min_value = sample->cell_voltage_v[i];
        }
    }
    return min_value;
}

uint32_t fault_manager_evaluate(const bms_sample_t *sample) {
    uint32_t faults = BMS_FAULT_NONE;

    for (int i = 0; i < BMS_CELL_COUNT; ++i) {
        if (sample->cell_voltage_v[i] > 4.25f) {
            faults |= BMS_FAULT_CELL_OVERVOLT;
        }
        if (sample->cell_voltage_v[i] < 3.00f) {
            faults |= BMS_FAULT_CELL_UNDERVOLT;
        }
    }

    for (int i = 0; i < BMS_TEMP_SENSOR_COUNT; ++i) {
        if (sample->temperatures_c[i] > 60.0f) {
            faults |= BMS_FAULT_OVERTEMP;
        }
        if (sample->temperatures_c[i] < -10.0f) {
            faults |= BMS_FAULT_UNDERTEMP;
        }
    }

    if (sample->pack_current_a > 45.0f || sample->pack_current_a < -35.0f) {
        faults |= BMS_FAULT_OVERCURRENT;
    }

    if ((cell_max(sample) - cell_min(sample)) > 0.080f) {
        faults |= BMS_FAULT_IMBALANCE;
    }

    return faults;
}

