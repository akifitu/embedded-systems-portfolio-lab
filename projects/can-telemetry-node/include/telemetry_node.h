#ifndef TELEMETRY_NODE_H
#define TELEMETRY_NODE_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "can_frame.h"

typedef enum {
    NODE_FAULT_NONE = 0,
    NODE_FAULT_UNDERVOLTAGE = 1u << 0,
    NODE_FAULT_OVERTEMP = 1u << 1,
    NODE_FAULT_CONTACTOR_OPEN = 1u << 2
} node_fault_t;

typedef struct {
    float pack_voltage_v;
    float pack_current_a;
    float board_temp_c;
    float soc_percent;
    bool contactor_closed;
} telemetry_sample_t;

typedef struct {
    uint32_t time_ms;
    uint32_t last_status_ms;
    uint32_t last_thermal_ms;
    uint32_t last_faults;
} telemetry_node_t;

void telemetry_node_init(telemetry_node_t *node);
size_t telemetry_node_step(telemetry_node_t *node,
                           const telemetry_sample_t *sample, uint32_t dt_ms,
                           can_frame_t *frames_out, size_t max_frames);
uint32_t telemetry_node_faults(const telemetry_sample_t *sample);

#endif

