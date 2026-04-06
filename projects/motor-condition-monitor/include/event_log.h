#ifndef EVENT_LOG_H
#define EVENT_LOG_H

#include <stddef.h>

#include "monitor_types.h"

typedef struct {
    mcm_event_t entries[MCM_EVENT_CAPACITY];
    size_t head;
    size_t count;
} mcm_event_log_t;

void mcm_event_log_init(mcm_event_log_t *log);
void mcm_event_log_push(mcm_event_log_t *log, const mcm_event_t *event);
const mcm_event_t *mcm_event_log_get(const mcm_event_log_t *log, size_t index);

#endif

