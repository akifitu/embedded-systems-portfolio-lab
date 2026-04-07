#ifndef FAILSAFE_TYPES_H
#define FAILSAFE_TYPES_H

#include <stdbool.h>
#include <stdint.h>

typedef enum {
    UAV_STATE_STANDBY = 0,
    UAV_STATE_MISSION,
    UAV_STATE_RTL,
    UAV_STATE_LAND,
    UAV_STATE_DISARMED
} uav_state_t;

typedef enum {
    UAV_COMMAND_HOLD = 0,
    UAV_COMMAND_PROCEED,
    UAV_COMMAND_RETURN_HOME,
    UAV_COMMAND_DESCEND,
    UAV_COMMAND_CUT_MOTORS
} uav_command_t;

typedef enum {
    UAV_REASON_NONE = 0,
    UAV_REASON_ARMING_BLOCK,
    UAV_REASON_GEOFENCE,
    UAV_REASON_LINK_LOSS,
    UAV_REASON_RETURN_MARGIN,
    UAV_REASON_NAV_LOSS,
    UAV_REASON_CRITICAL_BATTERY
} uav_reason_t;

typedef enum {
    UAV_HEALTH_GREEN = 0,
    UAV_HEALTH_YELLOW,
    UAV_HEALTH_RED
} uav_health_t;

typedef struct {
    bool armed;
    bool rc_link_ok;
    bool gps_ok;
    uint8_t gps_satellites;
    uint8_t battery_pct;
    uint16_t distance_home_m;
    uint16_t altitude_m;
    uint16_t geofence_radius_m;
    uint16_t wind_dmps;
} uav_input_t;

typedef struct {
    uav_state_t state;
    uav_command_t command;
    uav_reason_t reason;
    uav_health_t health;
    uint8_t reserve_margin_pct;
    uint8_t return_budget_pct;
    uint8_t link_loss_count;
    bool geofence_breached;
} uav_output_t;

const char *uav_state_name(uav_state_t state);
const char *uav_command_name(uav_command_t command);
const char *uav_reason_name(uav_reason_t reason);
const char *uav_health_name(uav_health_t health);

#endif
