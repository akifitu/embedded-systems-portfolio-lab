#ifndef ENERGY_BUDGET_H
#define ENERGY_BUDGET_H

#include <stdint.h>

#include "node_types.h"

uint16_t ehn_energy_apply(uint16_t battery_mwh, uint16_t harvested_mw,
                          ehn_action_t action, uint16_t wake_seconds);

#endif

