#ifndef JOURNAL_H
#define JOURNAL_H

#include <stdbool.h>
#include <stddef.h>

#include "flash_sim.h"

typedef struct {
    rfj_flash_t *flash;
    size_t write_page;
    size_t write_slot;
    uint32_t next_sequence;
    size_t valid_records;
    bool has_latest;
    rfj_record_view_t latest;
} rfj_journal_t;

void rfj_journal_format(rfj_journal_t *journal, rfj_flash_t *flash);
void rfj_journal_mount(rfj_journal_t *journal, rfj_flash_t *flash);
bool rfj_journal_append(rfj_journal_t *journal, const rfj_event_t *event);
bool rfj_journal_append_torn(rfj_journal_t *journal, const rfj_event_t *event);
size_t rfj_journal_replay(const rfj_journal_t *journal,
                          rfj_record_view_t *records_out, size_t max_records);

#endif

