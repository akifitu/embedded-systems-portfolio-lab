#include <stdio.h>

#include "journal.h"

static rfj_event_t make_event(uint32_t timestamp_ms, rfj_event_type_t type,
                              rfj_severity_t severity, uint16_t value) {
    rfj_event_t event = {timestamp_ms, type, severity, value};
    return event;
}

static void append_event(rfj_journal_t *journal, uint32_t timestamp_ms,
                         rfj_event_type_t type, rfj_severity_t severity,
                         uint16_t value) {
    rfj_event_t event = make_event(timestamp_ms, type, severity, value);
    rfj_journal_append(journal, &event);
}

static void append_event_torn(rfj_journal_t *journal, uint32_t timestamp_ms,
                              rfj_event_type_t type, rfj_severity_t severity,
                              uint16_t value) {
    rfj_event_t event = make_event(timestamp_ms, type, severity, value);
    rfj_journal_append_torn(journal, &event);
}

static void print_tail(const rfj_journal_t *journal) {
    rfj_record_view_t records[RFJ_TOTAL_SLOTS];
    size_t count = rfj_journal_replay(journal, records, RFJ_TOTAL_SLOTS);
    size_t start = (count > 3u) ? count - 3u : 0u;

    for (size_t i = start; i < count; ++i) {
        printf("tail seq=%u type=%s sev=%s value=%u\n", records[i].sequence,
               rfj_event_type_name(records[i].event.type),
               rfj_severity_name(records[i].event.severity),
               records[i].event.value);
    }
}

int main(void) {
    rfj_flash_t flash;
    rfj_journal_t journal;
    rfj_journal_t recovered;

    rfj_journal_format(&journal, &flash);
    append_event(&journal, 10u, RFJ_EVENT_BOOT, RFJ_SEVERITY_INFO, 1u);
    append_event(&journal, 40u, RFJ_EVENT_SENSOR_TIMEOUT, RFJ_SEVERITY_WARNING,
                 7u);
    append_event(&journal, 70u, RFJ_EVENT_OVERCURRENT, RFJ_SEVERITY_CRITICAL,
                 42u);
    append_event_torn(&journal, 95u, RFJ_EVENT_POWER_FAIL,
                      RFJ_SEVERITY_CRITICAL, 1u);

    rfj_journal_mount(&recovered, &flash);
    printf("boot_a valid=%zu latest_seq=%u latest_type=%s erase=[%u %u %u]\n",
           recovered.valid_records, recovered.latest.sequence,
           rfj_event_type_name(recovered.latest.event.type),
           rfj_flash_erase_cycles(&flash, 0u), rfj_flash_erase_cycles(&flash, 1u),
           rfj_flash_erase_cycles(&flash, 2u));

    append_event(&recovered, 120u, RFJ_EVENT_BOOT, RFJ_SEVERITY_INFO, 2u);
    append_event(&recovered, 150u, RFJ_EVENT_SENSOR_TIMEOUT,
                 RFJ_SEVERITY_WARNING, 12u);
    append_event(&recovered, 180u, RFJ_EVENT_OVERCURRENT,
                 RFJ_SEVERITY_CRITICAL, 55u);
    append_event(&recovered, 210u, RFJ_EVENT_BOOT, RFJ_SEVERITY_INFO, 3u);
    append_event(&recovered, 240u, RFJ_EVENT_SENSOR_TIMEOUT,
                 RFJ_SEVERITY_WARNING, 14u);
    append_event(&recovered, 270u, RFJ_EVENT_OVERCURRENT,
                 RFJ_SEVERITY_CRITICAL, 63u);
    append_event(&recovered, 300u, RFJ_EVENT_WATCHDOG_RESET,
                 RFJ_SEVERITY_WARNING, 1u);
    append_event(&recovered, 330u, RFJ_EVENT_POWER_FAIL, RFJ_SEVERITY_CRITICAL,
                 1u);
    append_event(&recovered, 360u, RFJ_EVENT_CONFIG_CHANGE, RFJ_SEVERITY_INFO,
                 7u);

    printf("boot_b valid=%zu latest_seq=%u latest_type=%s erase=[%u %u %u]\n",
           recovered.valid_records, recovered.latest.sequence,
           rfj_event_type_name(recovered.latest.event.type),
           rfj_flash_erase_cycles(&flash, 0u), rfj_flash_erase_cycles(&flash, 1u),
           rfj_flash_erase_cycles(&flash, 2u));
    print_tail(&recovered);

    return 0;
}
