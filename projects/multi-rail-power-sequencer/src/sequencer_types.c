#include "sequencer_types.h"

#include <stdio.h>
#include <string.h>

const char *pseq_state_name(pseq_state_t state) {
    switch (state) {
        case PSEQ_STATE_IDLE:
            return "IDLE";
        case PSEQ_STATE_STARTUP:
            return "STARTUP";
        case PSEQ_STATE_STABLE:
            return "STABLE";
        case PSEQ_STATE_RETRY_WAIT:
            return "RETRY_WAIT";
        case PSEQ_STATE_SHUTDOWN:
            return "SHUTDOWN";
        case PSEQ_STATE_FAULT:
            return "FAULT";
    }

    return "UNKNOWN";
}

const char *pseq_rail_name(int rail_index) {
    switch (rail_index) {
        case PSEQ_RAIL_CORE_1V0:
            return "CORE_1V0";
        case PSEQ_RAIL_IO_1V8:
            return "IO_1V8";
        case PSEQ_RAIL_AUX_3V3:
            return "AUX_3V3";
        case PSEQ_RAIL_RADIO_5V:
            return "RADIO_5V";
        default:
            return "NONE";
    }
}

static void append_fault(char *buffer, unsigned buffer_len, const char *label) {
    unsigned used = (unsigned)strlen(buffer);

    if (used >= buffer_len) {
        return;
    }

    if (used != 0u) {
        (void)snprintf(buffer + used, buffer_len - used, "|%s", label);
    } else {
        (void)snprintf(buffer + used, buffer_len - used, "%s", label);
    }
}

void pseq_faults_to_string(uint32_t faults, char *buffer, unsigned buffer_len) {
    if (buffer_len == 0u) {
        return;
    }

    buffer[0] = '\0';
    if (faults == PSEQ_FAULT_NONE) {
        (void)snprintf(buffer, buffer_len, "none");
        return;
    }

    if ((faults & PSEQ_FAULT_PG_TIMEOUT) != 0u) {
        append_fault(buffer, buffer_len, "pg_timeout");
    }
    if ((faults & PSEQ_FAULT_BROWNOUT) != 0u) {
        append_fault(buffer, buffer_len, "brownout");
    }
    if ((faults & PSEQ_FAULT_SHORT_CIRCUIT) != 0u) {
        append_fault(buffer, buffer_len, "short_circuit");
    }
    if ((faults & PSEQ_FAULT_RETRY_EXHAUSTED) != 0u) {
        append_fault(buffer, buffer_len, "retry_exhausted");
    }
}
