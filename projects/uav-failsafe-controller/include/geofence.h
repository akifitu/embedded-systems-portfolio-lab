#ifndef GEOFENCE_H
#define GEOFENCE_H

#include <stdbool.h>
#include <stdint.h>

typedef struct {
    bool breached;
    uint16_t overrun_m;
    uint16_t buffer_m;
} geofence_status_t;

geofence_status_t geofence_evaluate(uint16_t distance_home_m, uint16_t radius_m);

#endif
