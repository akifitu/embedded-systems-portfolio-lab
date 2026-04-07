#include "geofence.h"

geofence_status_t geofence_evaluate(uint16_t distance_home_m, uint16_t radius_m) {
    geofence_status_t status;

    status.breached = distance_home_m > radius_m;
    if (status.breached) {
        status.overrun_m = (uint16_t)(distance_home_m - radius_m);
        status.buffer_m = 0u;
    } else {
        status.overrun_m = 0u;
        status.buffer_m = (uint16_t)(radius_m - distance_home_m);
    }
    return status;
}
