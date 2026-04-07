#ifndef SAFETY_MONITOR_H
#define SAFETY_MONITOR_H

#include "pump_types.h"

pump_fault_t safety_monitor_fault(const pump_input_t *input);

#endif
