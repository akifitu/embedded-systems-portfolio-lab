#include <stdio.h>

#include "journal.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static rfj_event_t make_event(uint32_t timestamp_ms, rfj_event_type_t type,
                              rfj_severity_t severity, uint16_t value) {
    rfj_event_t event = {timestamp_ms, type, severity, value};
    return event;
}

static bool append_event(rfj_journal_t *journal, uint32_t timestamp_ms,
                         rfj_event_type_t type, rfj_severity_t severity,
                         uint16_t value) {
    rfj_event_t event = make_event(timestamp_ms, type, severity, value);
    return rfj_journal_append(journal, &event);
}

static bool append_event_torn(rfj_journal_t *journal, uint32_t timestamp_ms,
                              rfj_event_type_t type, rfj_severity_t severity,
                              uint16_t value) {
    rfj_event_t event = make_event(timestamp_ms, type, severity, value);
    return rfj_journal_append_torn(journal, &event);
}

static int test_clean_mount_recovers_latest(void) {
    rfj_flash_t flash;
    rfj_journal_t journal;
    rfj_journal_t mounted;

    rfj_journal_format(&journal, &flash);
    ASSERT_TRUE(append_event(&journal, 10u, RFJ_EVENT_BOOT, RFJ_SEVERITY_INFO,
                             1u));
    ASSERT_TRUE(append_event(&journal, 20u, RFJ_EVENT_SENSOR_TIMEOUT,
                             RFJ_SEVERITY_WARNING, 9u));

    rfj_journal_mount(&mounted, &flash);
    ASSERT_TRUE(mounted.valid_records == 2u);
    ASSERT_TRUE(mounted.has_latest);
    ASSERT_TRUE(mounted.latest.sequence == 1u);
    ASSERT_TRUE(mounted.latest.event.type == RFJ_EVENT_SENSOR_TIMEOUT);
    return 0;
}

static int test_torn_write_is_ignored(void) {
    rfj_flash_t flash;
    rfj_journal_t journal;
    rfj_journal_t mounted;

    rfj_journal_format(&journal, &flash);
    ASSERT_TRUE(append_event(&journal, 10u, RFJ_EVENT_BOOT, RFJ_SEVERITY_INFO,
                             1u));
    ASSERT_TRUE(append_event_torn(&journal, 15u, RFJ_EVENT_POWER_FAIL,
                                  RFJ_SEVERITY_CRITICAL, 1u));

    rfj_journal_mount(&mounted, &flash);
    ASSERT_TRUE(mounted.valid_records == 1u);
    ASSERT_TRUE(mounted.latest.sequence == 0u);
    ASSERT_TRUE(mounted.latest.event.type == RFJ_EVENT_BOOT);
    return 0;
}

static int test_rotation_erases_page(void) {
    rfj_flash_t flash;
    rfj_journal_t journal;
    rfj_journal_t mounted;

    rfj_journal_format(&journal, &flash);

    for (uint32_t i = 0u; i < 13u; ++i) {
        ASSERT_TRUE(append_event(&journal, i * 10u, RFJ_EVENT_BOOT,
                                 RFJ_SEVERITY_INFO, (uint16_t)i));
    }

    ASSERT_TRUE(rfj_flash_erase_cycles(&flash, 0u) == 1u);
    rfj_journal_mount(&mounted, &flash);
    ASSERT_TRUE(mounted.has_latest);
    ASSERT_TRUE(mounted.latest.sequence == 12u);
    ASSERT_TRUE(mounted.latest.event.value == 12u);
    return 0;
}

static int test_replay_is_sorted(void) {
    rfj_flash_t flash;
    rfj_journal_t journal;
    rfj_record_view_t records[RFJ_TOTAL_SLOTS];
    size_t count;

    rfj_journal_format(&journal, &flash);
    ASSERT_TRUE(append_event(&journal, 10u, RFJ_EVENT_BOOT, RFJ_SEVERITY_INFO,
                             1u));
    ASSERT_TRUE(append_event(&journal, 20u, RFJ_EVENT_WATCHDOG_RESET,
                             RFJ_SEVERITY_WARNING, 2u));
    ASSERT_TRUE(append_event(&journal, 30u, RFJ_EVENT_CONFIG_CHANGE,
                             RFJ_SEVERITY_INFO, 3u));

    count = rfj_journal_replay(&journal, records, RFJ_TOTAL_SLOTS);
    ASSERT_TRUE(count == 3u);
    ASSERT_TRUE(records[0].sequence == 0u);
    ASSERT_TRUE(records[1].sequence == 1u);
    ASSERT_TRUE(records[2].sequence == 2u);
    ASSERT_TRUE(records[2].event.type == RFJ_EVENT_CONFIG_CHANGE);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_clean_mount_recovers_latest() == 0);
    ASSERT_TRUE(test_torn_write_is_ignored() == 0);
    ASSERT_TRUE(test_rotation_erases_page() == 0);
    ASSERT_TRUE(test_replay_is_sorted() == 0);
    printf("resilient-flash-journal tests passed\n");
    return 0;
}
