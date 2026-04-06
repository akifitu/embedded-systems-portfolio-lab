#include "journal_types.h"

const char *rfj_event_type_name(rfj_event_type_t type) {
    switch (type) {
        case RFJ_EVENT_BOOT:
            return "BOOT";
        case RFJ_EVENT_WATCHDOG_RESET:
            return "WATCHDOG_RESET";
        case RFJ_EVENT_SENSOR_TIMEOUT:
            return "SENSOR_TIMEOUT";
        case RFJ_EVENT_OVERCURRENT:
            return "OVERCURRENT";
        case RFJ_EVENT_POWER_FAIL:
            return "POWER_FAIL";
        case RFJ_EVENT_CONFIG_CHANGE:
            return "CONFIG_CHANGE";
        default:
            return "UNKNOWN";
    }
}

const char *rfj_severity_name(rfj_severity_t severity) {
    switch (severity) {
        case RFJ_SEVERITY_INFO:
            return "INFO";
        case RFJ_SEVERITY_WARNING:
            return "WARNING";
        case RFJ_SEVERITY_CRITICAL:
            return "CRITICAL";
        default:
            return "UNKNOWN";
    }
}

