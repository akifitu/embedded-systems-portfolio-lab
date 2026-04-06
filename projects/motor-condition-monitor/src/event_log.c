#include "event_log.h"

void mcm_event_log_init(mcm_event_log_t *log) {
    log->head = 0u;
    log->count = 0u;
}

void mcm_event_log_push(mcm_event_log_t *log, const mcm_event_t *event) {
    size_t target_index = (log->head + log->count) % MCM_EVENT_CAPACITY;

    if (log->count == MCM_EVENT_CAPACITY) {
        log->entries[log->head] = *event;
        log->head = (log->head + 1u) % MCM_EVENT_CAPACITY;
        return;
    }

    log->entries[target_index] = *event;
    log->count++;
}

const mcm_event_t *mcm_event_log_get(const mcm_event_log_t *log, size_t index) {
    if (index >= log->count) {
        return 0;
    }

    return &log->entries[(log->head + index) % MCM_EVENT_CAPACITY];
}

