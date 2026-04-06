#include "condition_monitor.h"

#include "fault_classifier.h"
#include "feature_extractor.h"

static void copy_window(const mcm_monitor_t *monitor,
                        mcm_sample_t ordered_window[MCM_WINDOW_SIZE]) {
    size_t start_index = (monitor->count < MCM_WINDOW_SIZE) ? 0u : monitor->next_index;

    for (size_t i = 0; i < MCM_WINDOW_SIZE; ++i) {
        ordered_window[i] = monitor->window[(start_index + i) % MCM_WINDOW_SIZE];
    }
}

void mcm_monitor_init(mcm_monitor_t *monitor) {
    monitor->next_index = 0u;
    monitor->count = 0u;
    monitor->active_fault = MCM_FAULT_HEALTHY;
    monitor->last_features = (mcm_features_t){0};
    mcm_event_log_init(&monitor->event_log);
}

mcm_analysis_t mcm_monitor_push_sample(mcm_monitor_t *monitor,
                                       const mcm_sample_t *sample) {
    mcm_analysis_t analysis = {0};
    mcm_sample_t ordered_window[MCM_WINDOW_SIZE];

    monitor->window[monitor->next_index] = *sample;
    monitor->next_index = (monitor->next_index + 1u) % MCM_WINDOW_SIZE;

    if (monitor->count < MCM_WINDOW_SIZE) {
        monitor->count++;
    }

    analysis.active_fault = monitor->active_fault;
    analysis.logged_events = monitor->event_log.count;

    if (monitor->count < MCM_WINDOW_SIZE) {
        return analysis;
    }

    if (monitor->next_index != 0u) {
        return analysis;
    }

    copy_window(monitor, ordered_window);
    mcm_features_compute(ordered_window, MCM_WINDOW_SIZE, &analysis.features);
    analysis.window_ready = true;
    analysis.active_fault = mcm_classify_fault(&analysis.features);

    if (analysis.active_fault != monitor->active_fault) {
        mcm_event_t event = {
            .timestamp_ms = sample->timestamp_ms,
            .fault = analysis.active_fault,
            .severity = mcm_fault_severity(analysis.active_fault),
            .snapshot = analysis.features,
        };

        mcm_event_log_push(&monitor->event_log, &event);
        monitor->active_fault = analysis.active_fault;
        analysis.fault_changed = true;
    }

    monitor->last_features = analysis.features;
    analysis.active_fault = monitor->active_fault;
    analysis.logged_events = monitor->event_log.count;
    return analysis;
}

const mcm_event_log_t *mcm_monitor_event_log(const mcm_monitor_t *monitor) {
    return &monitor->event_log;
}
