#include "flash_sim.h"

#include <string.h>

void rfj_flash_init(rfj_flash_t *flash) {
    memset(flash->pages, 0xFF, sizeof(flash->pages));
    memset(flash->erase_count, 0, sizeof(flash->erase_count));
}

void rfj_flash_erase_page(rfj_flash_t *flash, size_t page) {
    memset(flash->pages[page], 0xFF, sizeof(flash->pages[page]));
    flash->erase_count[page]++;
}

bool rfj_flash_is_page_blank(const rfj_flash_t *flash, size_t page) {
    for (size_t slot = 0; slot < RFJ_SLOTS_PER_PAGE; ++slot) {
        if (!rfj_flash_is_slot_blank(flash, page, slot)) {
            return false;
        }
    }
    return true;
}

bool rfj_flash_is_slot_blank(const rfj_flash_t *flash, size_t page, size_t slot) {
    for (size_t i = 0; i < RFJ_SLOT_SIZE; ++i) {
        if (flash->pages[page][slot][i] != 0xFFu) {
            return false;
        }
    }
    return true;
}

uint32_t rfj_flash_erase_cycles(const rfj_flash_t *flash, size_t page) {
    return flash->erase_count[page];
}

uint8_t *rfj_flash_slot(rfj_flash_t *flash, size_t page, size_t slot) {
    return flash->pages[page][slot];
}

const uint8_t *rfj_flash_slot_const(const rfj_flash_t *flash, size_t page,
                                    size_t slot) {
    return flash->pages[page][slot];
}

