#include "precharge_monitor.h"

#define PRECHARGE_READY_DELTA_MV 250u
#define PRECHARGE_MISMATCH_DELTA_MV 3000u

unsigned precharge_delta_mv(const swap_dock_input_t *input) {
    if (!input->pack_present) {
        return 0u;
    }

    if (input->pack_voltage_mv >= input->bus_voltage_mv) {
        return input->pack_voltage_mv - input->bus_voltage_mv;
    }
    return input->bus_voltage_mv - input->pack_voltage_mv;
}

bool precharge_ready(unsigned delta_mv) {
    return delta_mv <= PRECHARGE_READY_DELTA_MV;
}

bool precharge_voltage_mismatch(unsigned delta_mv) {
    return delta_mv > PRECHARGE_MISMATCH_DELTA_MV;
}
