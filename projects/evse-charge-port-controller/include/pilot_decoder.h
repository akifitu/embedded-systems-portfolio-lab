#ifndef PILOT_DECODER_H
#define PILOT_DECODER_H

#include <stdint.h>

#include "evse_types.h"

typedef struct {
    evse_pilot_state_t pilot_state;
    uint16_t advertised_current_a_x10;
} pilot_decode_t;

pilot_decode_t pilot_decoder_decode(int16_t pilot_voltage_dv,
                                    uint16_t pwm_duty_pct_x10);

#endif
