#ifndef FLASH_SIM_H
#define FLASH_SIM_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "journal_types.h"

typedef struct {
    uint8_t pages[RFJ_PAGE_COUNT][RFJ_SLOTS_PER_PAGE][RFJ_SLOT_SIZE];
    uint32_t erase_count[RFJ_PAGE_COUNT];
} rfj_flash_t;

void rfj_flash_init(rfj_flash_t *flash);
void rfj_flash_erase_page(rfj_flash_t *flash, size_t page);
bool rfj_flash_is_page_blank(const rfj_flash_t *flash, size_t page);
bool rfj_flash_is_slot_blank(const rfj_flash_t *flash, size_t page, size_t slot);
uint32_t rfj_flash_erase_cycles(const rfj_flash_t *flash, size_t page);
uint8_t *rfj_flash_slot(rfj_flash_t *flash, size_t page, size_t slot);
const uint8_t *rfj_flash_slot_const(const rfj_flash_t *flash, size_t page,
                                    size_t slot);

#endif

