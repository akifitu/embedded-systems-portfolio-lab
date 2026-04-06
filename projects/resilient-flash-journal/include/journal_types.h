#ifndef JOURNAL_TYPES_H
#define JOURNAL_TYPES_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#define RFJ_PAGE_COUNT 3u
#define RFJ_SLOTS_PER_PAGE 4u
#define RFJ_TOTAL_SLOTS (RFJ_PAGE_COUNT * RFJ_SLOTS_PER_PAGE)
#define RFJ_SLOT_SIZE 20u

typedef enum {
    RFJ_EVENT_BOOT = 0,
    RFJ_EVENT_WATCHDOG_RESET,
    RFJ_EVENT_SENSOR_TIMEOUT,
    RFJ_EVENT_OVERCURRENT,
    RFJ_EVENT_POWER_FAIL,
    RFJ_EVENT_CONFIG_CHANGE
} rfj_event_type_t;

typedef enum {
    RFJ_SEVERITY_INFO = 0,
    RFJ_SEVERITY_WARNING,
    RFJ_SEVERITY_CRITICAL
} rfj_severity_t;

typedef struct {
    uint32_t timestamp_ms;
    rfj_event_type_t type;
    rfj_severity_t severity;
    uint16_t value;
} rfj_event_t;

typedef struct {
    uint32_t sequence;
    rfj_event_t event;
} rfj_record_view_t;

const char *rfj_event_type_name(rfj_event_type_t type);
const char *rfj_severity_name(rfj_severity_t severity);

#endif

