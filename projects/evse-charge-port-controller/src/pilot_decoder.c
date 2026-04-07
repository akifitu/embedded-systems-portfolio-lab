#include "pilot_decoder.h"

static uint16_t decode_current_a_x10(uint16_t pwm_duty_pct_x10) {
    return (uint16_t)((pwm_duty_pct_x10 * 6u + 5u) / 10u);
}

pilot_decode_t pilot_decoder_decode(int16_t pilot_voltage_dv,
                                    uint16_t pwm_duty_pct_x10) {
    pilot_decode_t decoded;

    decoded.advertised_current_a_x10 = 0u;
    if (pilot_voltage_dv >= 105) {
        decoded.pilot_state = EVSE_PILOT_A;
        return decoded;
    }

    if (pilot_voltage_dv >= 75) {
        decoded.pilot_state = EVSE_PILOT_B;
        decoded.advertised_current_a_x10 = decode_current_a_x10(pwm_duty_pct_x10);
        return decoded;
    }

    if (pilot_voltage_dv >= 45) {
        decoded.pilot_state = EVSE_PILOT_C;
        decoded.advertised_current_a_x10 = decode_current_a_x10(pwm_duty_pct_x10);
        return decoded;
    }

    decoded.pilot_state = EVSE_PILOT_F;
    return decoded;
}
