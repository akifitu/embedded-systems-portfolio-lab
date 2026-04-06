#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

#include <stdint.h>

#include "bms_types.h"

uint32_t fault_manager_evaluate(const bms_sample_t *sample);

#endif

