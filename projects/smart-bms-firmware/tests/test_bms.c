#include <math.h>
#include <stdio.h>

#include "bms_controller.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static int test_charge_state(void) {
    bms_controller_t controller;
    bms_controller_init(&controller, 20.0f, 50.0f);

    bms_sample_t sample = {
        .pack_current_a = -10.0f,
        .cell_voltage_v = {4.10f, 4.08f, 4.09f, 4.12f},
        .temperatures_c = {25.0f, 25.0f},
        .dt_seconds = 1.0f,
    };

    bms_output_t output = bms_controller_step(&controller, &sample);
    ASSERT_TRUE(output.state == BMS_STATE_CHARGING);
    ASSERT_TRUE(output.charge_enabled);
    ASSERT_TRUE(!output.discharge_enabled);
    return 0;
}

static int test_fault_transition(void) {
    bms_controller_t controller;
    bms_controller_init(&controller, 20.0f, 50.0f);

    bms_sample_t sample = {
        .pack_current_a = 4.0f,
        .cell_voltage_v = {3.90f, 3.91f, 3.92f, 3.93f},
        .temperatures_c = {25.0f, 70.0f},
        .dt_seconds = 1.0f,
    };

    bms_output_t output = bms_controller_step(&controller, &sample);
    ASSERT_TRUE(output.state == BMS_STATE_FAULT);
    ASSERT_TRUE((output.faults & BMS_FAULT_OVERTEMP) != 0u);
    ASSERT_TRUE(!output.charge_enabled);
    return 0;
}

static int test_balancing_request(void) {
    bms_controller_t controller;
    bms_controller_init(&controller, 20.0f, 70.0f);

    bms_sample_t sample = {
        .pack_current_a = 0.0f,
        .cell_voltage_v = {4.06f, 4.08f, 4.07f, 4.14f},
        .temperatures_c = {24.0f, 24.0f},
        .dt_seconds = 1.0f,
    };

    bms_output_t output = bms_controller_step(&controller, &sample);
    ASSERT_TRUE(output.state == BMS_STATE_IDLE);
    ASSERT_TRUE(output.balancing_enabled[3]);
    ASSERT_TRUE(!output.balancing_enabled[0]);
    return 0;
}

static int test_soc_drop_on_discharge(void) {
    bms_controller_t controller;
    bms_controller_init(&controller, 10.0f, 80.0f);

    bms_sample_t sample = {
        .pack_current_a = 18.0f,
        .cell_voltage_v = {3.95f, 3.95f, 3.95f, 3.95f},
        .temperatures_c = {23.0f, 23.0f},
        .dt_seconds = 60.0f,
    };

    bms_output_t output = bms_controller_step(&controller, &sample);
    ASSERT_TRUE(output.state == BMS_STATE_DISCHARGING);
    ASSERT_TRUE(fabsf(output.soc_percent - 77.0f) < 0.05f);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_charge_state() == 0);
    ASSERT_TRUE(test_fault_transition() == 0);
    ASSERT_TRUE(test_balancing_request() == 0);
    ASSERT_TRUE(test_soc_drop_on_discharge() == 0);
    printf("smart-bms-firmware tests passed\n");
    return 0;
}

