#include "dose_engine.h"

dose_engine_result_t dose_engine_advance(unsigned therapy_remaining_tenths,
                                         unsigned infusion_step_tenths) {
    dose_engine_result_t result;

    if (therapy_remaining_tenths <= infusion_step_tenths) {
        result.delivered_step_tenths = therapy_remaining_tenths;
        result.therapy_remaining_tenths = 0u;
        result.therapy_complete = true;
        return result;
    }

    result.delivered_step_tenths = infusion_step_tenths;
    result.therapy_remaining_tenths =
        therapy_remaining_tenths - infusion_step_tenths;
    result.therapy_complete = false;
    return result;
}
