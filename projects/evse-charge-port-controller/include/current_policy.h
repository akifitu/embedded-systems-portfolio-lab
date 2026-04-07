#ifndef CURRENT_POLICY_H
#define CURRENT_POLICY_H

#include <stdint.h>

uint16_t current_policy_limit_a_x10(uint16_t advertised_current_a_x10,
                                    uint16_t cable_limit_a_x10,
                                    int16_t temp_c_x10);

#endif
