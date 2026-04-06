#ifndef RAIL_PLAN_H
#define RAIL_PLAN_H

#include <stdint.h>

#include "sequencer_types.h"

typedef struct {
    const char *name;
    uint8_t pg_timeout_ticks;
} rail_plan_entry_t;

const rail_plan_entry_t *rail_plan_entry(int rail_index);

#endif
