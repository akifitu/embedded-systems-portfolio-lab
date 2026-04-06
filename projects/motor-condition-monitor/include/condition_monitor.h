#ifndef CONDITION_MONITOR_H
#define CONDITION_MONITOR_H

#include "event_log.h"

typedef struct {
    bool window_ready;
    bool fault_changed;
    mcm_fault_t active_fault;
    mcm_features_t features;
    size_t logged_events;
} mcm_analysis_t;

typedef struct {
    mcm_sample_t window[MCM_WINDOW_SIZE];
    size_t next_index;
    size_t count;
    mcm_fault_t active_fault;
    mcm_features_t last_features;
    mcm_event_log_t event_log;
} mcm_monitor_t;

void mcm_monitor_init(mcm_monitor_t *monitor);
mcm_analysis_t mcm_monitor_push_sample(mcm_monitor_t *monitor,
                                       const mcm_sample_t *sample);
const mcm_event_log_t *mcm_monitor_event_log(const mcm_monitor_t *monitor);

#endif

