#ifndef BMS_TYPES_H
#define BMS_TYPES_H

#include <stdbool.h>
#include <stdint.h>

#define BMS_CELL_COUNT 4
#define BMS_TEMP_SENSOR_COUNT 2

typedef enum {
    BMS_STATE_INIT = 0,
    BMS_STATE_IDLE,
    BMS_STATE_CHARGING,
    BMS_STATE_DISCHARGING,
    BMS_STATE_FAULT
} bms_state_t;

typedef enum {
    BMS_FAULT_NONE = 0,
    BMS_FAULT_CELL_OVERVOLT = 1u << 0,
    BMS_FAULT_CELL_UNDERVOLT = 1u << 1,
    BMS_FAULT_OVERTEMP = 1u << 2,
    BMS_FAULT_UNDERTEMP = 1u << 3,
    BMS_FAULT_OVERCURRENT = 1u << 4,
    BMS_FAULT_IMBALANCE = 1u << 5
} bms_fault_t;

typedef struct {
    float pack_current_a;
    float cell_voltage_v[BMS_CELL_COUNT];
    float temperatures_c[BMS_TEMP_SENSOR_COUNT];
    float dt_seconds;
} bms_sample_t;

typedef struct {
    bms_state_t state;
    uint32_t faults;
    float soc_percent;
    bool charge_enabled;
    bool discharge_enabled;
    bool balancing_enabled[BMS_CELL_COUNT];
} bms_output_t;

#endif

