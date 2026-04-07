#include "safety_monitor.h"

#define EMPTY_RESERVOIR_THRESHOLD_TENTHS 5u

pump_fault_t safety_monitor_fault(const pump_input_t *input) {
    if (input->air_in_line) {
        return PUMP_FAULT_AIR_IN_LINE;
    }
    if (input->occlusion_detected) {
        return PUMP_FAULT_OCCLUSION;
    }
    if (!input->door_closed) {
        return PUMP_FAULT_DOOR_OPEN;
    }
    if (input->reservoir_ml_tenths <= EMPTY_RESERVOIR_THRESHOLD_TENTHS) {
        return PUMP_FAULT_EMPTY_RESERVOIR;
    }
    return PUMP_FAULT_NONE;
}
