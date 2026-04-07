#ifndef PRECHARGE_MONITOR_H
#define PRECHARGE_MONITOR_H

#include <stdbool.h>

#include "swap_dock_types.h"

unsigned precharge_delta_mv(const swap_dock_input_t *input);
bool precharge_ready(unsigned delta_mv);
bool precharge_voltage_mismatch(unsigned delta_mv);

#endif
