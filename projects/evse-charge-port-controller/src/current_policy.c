#include "current_policy.h"

static uint16_t min_u16(uint16_t a, uint16_t b) {
    return (a < b) ? a : b;
}

uint16_t current_policy_limit_a_x10(uint16_t advertised_current_a_x10,
                                    uint16_t cable_limit_a_x10,
                                    int16_t temp_c_x10) {
    uint16_t limit;

    limit = min_u16(advertised_current_a_x10, cable_limit_a_x10);

    if ((temp_c_x10 >= 800) && (limit > 180u)) {
        limit = 180u;
    } else if ((temp_c_x10 >= 700) && (limit > 240u)) {
        limit = 240u;
    }

    return limit;
}
