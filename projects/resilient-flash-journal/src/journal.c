#include "journal.h"

#include <string.h>

#include "crc16.h"

#define RFJ_MAGIC 0xB16Bu
#define RFJ_COMMIT_TAG 0xA5u

enum {
    RFJ_OFFSET_MAGIC = 0,
    RFJ_OFFSET_SEQUENCE = 2,
    RFJ_OFFSET_TIMESTAMP = 6,
    RFJ_OFFSET_TYPE = 10,
    RFJ_OFFSET_SEVERITY = 11,
    RFJ_OFFSET_VALUE = 12,
    RFJ_OFFSET_CRC = 14,
    RFJ_OFFSET_COMMIT = 16,
    RFJ_OFFSET_DATA_END = 14
};

static void write_u16_le(uint8_t *buffer, size_t offset, uint16_t value) {
    buffer[offset] = (uint8_t)(value & 0xFFu);
    buffer[offset + 1u] = (uint8_t)((value >> 8u) & 0xFFu);
}

static void write_u32_le(uint8_t *buffer, size_t offset, uint32_t value) {
    buffer[offset] = (uint8_t)(value & 0xFFu);
    buffer[offset + 1u] = (uint8_t)((value >> 8u) & 0xFFu);
    buffer[offset + 2u] = (uint8_t)((value >> 16u) & 0xFFu);
    buffer[offset + 3u] = (uint8_t)((value >> 24u) & 0xFFu);
}

static uint16_t read_u16_le(const uint8_t *buffer, size_t offset) {
    return (uint16_t)(buffer[offset] | ((uint16_t)buffer[offset + 1u] << 8u));
}

static uint32_t read_u32_le(const uint8_t *buffer, size_t offset) {
    return (uint32_t)buffer[offset] |
           ((uint32_t)buffer[offset + 1u] << 8u) |
           ((uint32_t)buffer[offset + 2u] << 16u) |
           ((uint32_t)buffer[offset + 3u] << 24u);
}

static void advance_cursor(size_t *page, size_t *slot) {
    (*slot)++;
    if (*slot == RFJ_SLOTS_PER_PAGE) {
        *slot = 0u;
        *page = (*page + 1u) % RFJ_PAGE_COUNT;
    }
}

static void ensure_writable_cursor(rfj_journal_t *journal) {
    while (1) {
        if (journal->write_slot == 0u) {
            if (!rfj_flash_is_page_blank(journal->flash, journal->write_page)) {
                rfj_flash_erase_page(journal->flash, journal->write_page);
            }
            return;
        }

        if (rfj_flash_is_slot_blank(journal->flash, journal->write_page,
                                    journal->write_slot)) {
            return;
        }

        journal->write_page = (journal->write_page + 1u) % RFJ_PAGE_COUNT;
        journal->write_slot = 0u;
    }
}

static void encode_record(uint8_t slot[RFJ_SLOT_SIZE], uint32_t sequence,
                          const rfj_event_t *event, bool committed) {
    uint16_t crc16;

    memset(slot, 0xFF, RFJ_SLOT_SIZE);
    write_u16_le(slot, RFJ_OFFSET_MAGIC, RFJ_MAGIC);
    write_u32_le(slot, RFJ_OFFSET_SEQUENCE, sequence);
    write_u32_le(slot, RFJ_OFFSET_TIMESTAMP, event->timestamp_ms);
    slot[RFJ_OFFSET_TYPE] = (uint8_t)event->type;
    slot[RFJ_OFFSET_SEVERITY] = (uint8_t)event->severity;
    write_u16_le(slot, RFJ_OFFSET_VALUE, event->value);
    crc16 = rfj_crc16_compute(slot, RFJ_OFFSET_DATA_END);
    write_u16_le(slot, RFJ_OFFSET_CRC, crc16);
    if (committed) {
        slot[RFJ_OFFSET_COMMIT] = RFJ_COMMIT_TAG;
    }
}

static bool decode_record(const uint8_t slot[RFJ_SLOT_SIZE],
                          rfj_record_view_t *record_out) {
    uint16_t expected_crc;

    if (read_u16_le(slot, RFJ_OFFSET_MAGIC) != RFJ_MAGIC) {
        return false;
    }
    if (slot[RFJ_OFFSET_COMMIT] != RFJ_COMMIT_TAG) {
        return false;
    }

    expected_crc = rfj_crc16_compute(slot, RFJ_OFFSET_DATA_END);
    if (read_u16_le(slot, RFJ_OFFSET_CRC) != expected_crc) {
        return false;
    }

    if (record_out != 0) {
        record_out->sequence = read_u32_le(slot, RFJ_OFFSET_SEQUENCE);
        record_out->event.timestamp_ms = read_u32_le(slot, RFJ_OFFSET_TIMESTAMP);
        record_out->event.type = (rfj_event_type_t)slot[RFJ_OFFSET_TYPE];
        record_out->event.severity = (rfj_severity_t)slot[RFJ_OFFSET_SEVERITY];
        record_out->event.value = read_u16_le(slot, RFJ_OFFSET_VALUE);
    }

    return true;
}

static void collect_records(const rfj_journal_t *journal,
                            rfj_record_view_t records[RFJ_TOTAL_SLOTS],
                            size_t *count_out, size_t *latest_page_out,
                            size_t *latest_slot_out) {
    size_t count = 0u;
    uint32_t latest_sequence = 0u;
    bool found = false;

    for (size_t page = 0; page < RFJ_PAGE_COUNT; ++page) {
        for (size_t slot = 0; slot < RFJ_SLOTS_PER_PAGE; ++slot) {
            rfj_record_view_t view;

            if (!decode_record(
                    rfj_flash_slot_const(journal->flash, page, slot), &view)) {
                continue;
            }

            if (count < RFJ_TOTAL_SLOTS) {
                records[count++] = view;
            }

            if (!found || view.sequence > latest_sequence) {
                latest_sequence = view.sequence;
                *latest_page_out = page;
                *latest_slot_out = slot;
                found = true;
            }
        }
    }

    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1u; j < count; ++j) {
            if (records[j].sequence < records[i].sequence) {
                rfj_record_view_t temp = records[i];
                records[i] = records[j];
                records[j] = temp;
            }
        }
    }

    *count_out = count;
}

static bool append_internal(rfj_journal_t *journal, const rfj_event_t *event,
                            bool committed) {
    uint8_t slot_image[RFJ_SLOT_SIZE];

    ensure_writable_cursor(journal);
    encode_record(slot_image, journal->next_sequence, event, committed);
    memcpy(rfj_flash_slot(journal->flash, journal->write_page, journal->write_slot),
           slot_image, RFJ_SLOT_SIZE);

    journal->next_sequence++;
    advance_cursor(&journal->write_page, &journal->write_slot);

    if (committed) {
        rfj_journal_mount(journal, journal->flash);
    }

    return true;
}

void rfj_journal_format(rfj_journal_t *journal, rfj_flash_t *flash) {
    rfj_flash_init(flash);
    journal->flash = flash;
    journal->write_page = 0u;
    journal->write_slot = 0u;
    journal->next_sequence = 0u;
    journal->valid_records = 0u;
    journal->has_latest = false;
    journal->latest = (rfj_record_view_t){0};
}

void rfj_journal_mount(rfj_journal_t *journal, rfj_flash_t *flash) {
    rfj_record_view_t records[RFJ_TOTAL_SLOTS];
    size_t latest_page = 0u;
    size_t latest_slot = 0u;
    size_t count = 0u;

    journal->flash = flash;
    journal->write_page = 0u;
    journal->write_slot = 0u;
    journal->next_sequence = 0u;
    journal->valid_records = 0u;
    journal->has_latest = false;
    journal->latest = (rfj_record_view_t){0};

    collect_records(journal, records, &count, &latest_page, &latest_slot);
    journal->valid_records = count;

    if (count == 0u) {
        return;
    }

    journal->latest = records[count - 1u];
    journal->has_latest = true;
    journal->next_sequence = journal->latest.sequence + 1u;
    journal->write_page = latest_page;
    journal->write_slot = latest_slot;
    advance_cursor(&journal->write_page, &journal->write_slot);
}

bool rfj_journal_append(rfj_journal_t *journal, const rfj_event_t *event) {
    return append_internal(journal, event, true);
}

bool rfj_journal_append_torn(rfj_journal_t *journal, const rfj_event_t *event) {
    return append_internal(journal, event, false);
}

size_t rfj_journal_replay(const rfj_journal_t *journal,
                          rfj_record_view_t *records_out, size_t max_records) {
    rfj_record_view_t records[RFJ_TOTAL_SLOTS];
    size_t latest_page = 0u;
    size_t latest_slot = 0u;
    size_t count = 0u;
    size_t copy_count;

    collect_records(journal, records, &count, &latest_page, &latest_slot);
    copy_count = (count < max_records) ? count : max_records;

    for (size_t i = 0; i < copy_count; ++i) {
        records_out[i] = records[i];
    }

    return copy_count;
}
