#ifndef MONITOR_TYPES_H
#define MONITOR_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define MCM_WINDOW_SIZE 16u
#define MCM_EVENT_CAPACITY 8u

typedef enum {
    MCM_FAULT_HEALTHY = 0,
    MCM_FAULT_IMBALANCE,
    MCM_FAULT_BEARING_WEAR,
    MCM_FAULT_STALL,
    MCM_FAULT_OVERTEMP
} mcm_fault_t;

typedef enum {
    MCM_SEVERITY_INFO = 0,
    MCM_SEVERITY_WARNING,
    MCM_SEVERITY_CRITICAL
} mcm_severity_t;

typedef struct {
    int16_t vibration_mg;
    uint16_t motor_current_ma;
    int16_t board_temp_deci_c;
    uint32_t timestamp_ms;
} mcm_sample_t;

typedef struct {
    uint16_t rms_mg;
    uint16_t peak_to_peak_mg;
    uint16_t mean_abs_jerk_mg;
    uint16_t avg_current_ma;
    int16_t max_temp_deci_c;
} mcm_features_t;

typedef struct {
    uint32_t timestamp_ms;
    mcm_fault_t fault;
    mcm_severity_t severity;
    mcm_features_t snapshot;
} mcm_event_t;

#endif

