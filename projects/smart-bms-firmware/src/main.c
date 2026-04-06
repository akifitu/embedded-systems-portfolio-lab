#include <stdio.h>

#include "bms_controller.h"

static void print_faults(uint32_t faults) {
    if (faults == BMS_FAULT_NONE) {
        printf("none");
        return;
    }

    if (faults & BMS_FAULT_CELL_OVERVOLT) {
        printf(" cell_ov");
    }
    if (faults & BMS_FAULT_CELL_UNDERVOLT) {
        printf(" cell_uv");
    }
    if (faults & BMS_FAULT_OVERTEMP) {
        printf(" overtemp");
    }
    if (faults & BMS_FAULT_UNDERTEMP) {
        printf(" undertemp");
    }
    if (faults & BMS_FAULT_OVERCURRENT) {
        printf(" overcurrent");
    }
    if (faults & BMS_FAULT_IMBALANCE) {
        printf(" imbalance");
    }
}

static void print_balancing(const bms_output_t *output) {
    printf("[");
    for (int i = 0; i < BMS_CELL_COUNT; ++i) {
        printf("%d", output->balancing_enabled[i] ? 1 : 0);
        if (i < BMS_CELL_COUNT - 1) {
            printf(" ");
        }
    }
    printf("]");
}

int main(void) {
    bms_controller_t controller;
    bms_sample_t samples[] = {
        {
            .pack_current_a = 0.0f,
            .cell_voltage_v = {3.98f, 4.02f, 4.01f, 4.00f},
            .temperatures_c = {24.0f, 24.5f},
            .dt_seconds = 1.0f,
        },
        {
            .pack_current_a = -12.0f,
            .cell_voltage_v = {4.08f, 4.11f, 4.09f, 4.16f},
            .temperatures_c = {25.0f, 25.0f},
            .dt_seconds = 1.0f,
        },
        {
            .pack_current_a = -8.0f,
            .cell_voltage_v = {4.11f, 4.10f, 4.09f, 4.13f},
            .temperatures_c = {26.0f, 26.5f},
            .dt_seconds = 1.0f,
        },
        {
            .pack_current_a = 18.0f,
            .cell_voltage_v = {4.02f, 4.01f, 3.99f, 4.00f},
            .temperatures_c = {28.0f, 28.0f},
            .dt_seconds = 1.0f,
        },
        {
            .pack_current_a = 0.5f,
            .cell_voltage_v = {3.95f, 3.94f, 3.92f, 3.91f},
            .temperatures_c = {62.0f, 40.0f},
            .dt_seconds = 1.0f,
        },
    };
    bms_controller_init(&controller, 18.0f, 72.0f);

    for (int i = 0; i < 5; ++i) {
        bms_output_t output = bms_controller_step(&controller, &samples[i]);

        printf("step=%d state=%s soc=%.2f charge=%d discharge=%d faults=",
               i, bms_state_name(output.state), output.soc_percent,
               output.charge_enabled, output.discharge_enabled);
        print_faults(output.faults);
        printf(" balancing=");
        print_balancing(&output);
        printf("\n");
    }

    return 0;
}
