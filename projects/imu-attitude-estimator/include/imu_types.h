#ifndef IMU_TYPES_H
#define IMU_TYPES_H

#include <stdint.h>

typedef enum {
    IAE_STATE_STABLE = 0,
    IAE_STATE_TRACKING,
    IAE_STATE_VIBRATION,
    IAE_STATE_FREEFALL
} iae_motion_state_t;

typedef struct {
    float accel_g[3];
    float gyro_dps[3];
    float dt_s;
} iae_sample_t;

typedef struct {
    float roll_deg;
    float pitch_deg;
    float accel_norm_g;
    uint8_t confidence_percent;
    iae_motion_state_t motion_state;
} iae_output_t;

const char *iae_motion_state_name(iae_motion_state_t state);

#endif

