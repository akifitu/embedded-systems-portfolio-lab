#ifndef GPSDO_TYPES_H
#define GPSDO_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    GPSDO_STATE_ACQUIRE = 0,
    GPSDO_STATE_TRACKING,
    GPSDO_STATE_HOLDOVER,
    GPSDO_STATE_FAULT
} gpsdo_state_t;

typedef enum {
    GPSDO_QUALITY_SEARCH = 0,
    GPSDO_QUALITY_LOCKED,
    GPSDO_QUALITY_HOLDOVER,
    GPSDO_QUALITY_FAULT
} gpsdo_quality_t;

typedef struct {
    bool pps_valid;
    int32_t phase_error_ns;
    int16_t temperature_c_x10;
} gpsdo_input_t;

typedef struct {
    gpsdo_state_t state;
    gpsdo_quality_t quality;
    uint16_t trim_code;
    int32_t estimated_phase_ns;
    uint32_t holdover_uncertainty_ns;
} gpsdo_output_t;

const char *gpsdo_state_name(gpsdo_state_t state);
const char *gpsdo_quality_name(gpsdo_quality_t quality);

#endif
