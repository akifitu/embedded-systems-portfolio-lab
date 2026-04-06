#ifndef SEQUENCER_TYPES_H
#define SEQUENCER_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define PSEQ_RAIL_COUNT 4u
#define PSEQ_RAIL_NONE (-1)

typedef enum {
    PSEQ_STATE_IDLE = 0,
    PSEQ_STATE_STARTUP,
    PSEQ_STATE_STABLE,
    PSEQ_STATE_RETRY_WAIT,
    PSEQ_STATE_SHUTDOWN,
    PSEQ_STATE_FAULT
} pseq_state_t;

typedef enum {
    PSEQ_RAIL_CORE_1V0 = 0,
    PSEQ_RAIL_IO_1V8,
    PSEQ_RAIL_AUX_3V3,
    PSEQ_RAIL_RADIO_5V
} pseq_rail_t;

enum {
    PSEQ_FAULT_NONE = 0u,
    PSEQ_FAULT_PG_TIMEOUT = 1u << 0,
    PSEQ_FAULT_BROWNOUT = 1u << 1,
    PSEQ_FAULT_SHORT_CIRCUIT = 1u << 2,
    PSEQ_FAULT_RETRY_EXHAUSTED = 1u << 3
};

typedef struct {
    bool enable_request;
    bool shutdown_request;
    bool clear_fault;
} pseq_command_t;

typedef struct {
    bool rail_pg[PSEQ_RAIL_COUNT];
    bool brownout;
    bool short_circuit;
} pseq_observation_t;

typedef struct {
    pseq_state_t state;
    int current_rail;
    uint8_t enabled_mask;
    uint8_t retry_count;
    uint8_t progress_percent;
    uint32_t faults;
} pseq_output_t;

const char *pseq_state_name(pseq_state_t state);
const char *pseq_rail_name(int rail_index);
void pseq_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len);

#endif
