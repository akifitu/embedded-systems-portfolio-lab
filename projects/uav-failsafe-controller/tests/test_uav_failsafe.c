#include <assert.h>

#include "uav_failsafe_controller.h"

static void test_arming_block_when_gps_missing(void) {
    uav_failsafe_controller_t controller;
    uav_input_t input;
    uav_output_t output;

    uav_failsafe_controller_init(&controller);
    input.armed = false;
    input.rc_link_ok = true;
    input.gps_ok = false;
    input.gps_satellites = 0u;
    input.battery_pct = 48u;
    input.distance_home_m = 0u;
    input.altitude_m = 0u;
    input.geofence_radius_m = 400u;
    input.wind_dmps = 10u;
    output = uav_failsafe_controller_step(&controller, &input);

    assert(output.state == UAV_STATE_STANDBY);
    assert(output.command == UAV_COMMAND_HOLD);
    assert(output.reason == UAV_REASON_ARMING_BLOCK);
}

static void test_geofence_breach_enters_rtl(void) {
    uav_failsafe_controller_t controller;
    uav_input_t input;
    uav_output_t output;

    uav_failsafe_controller_init(&controller);
    input.armed = true;
    input.rc_link_ok = true;
    input.gps_ok = true;
    input.gps_satellites = 11u;
    input.battery_pct = 78u;
    input.distance_home_m = 820u;
    input.altitude_m = 70u;
    input.geofence_radius_m = 600u;
    input.wind_dmps = 50u;
    output = uav_failsafe_controller_step(&controller, &input);

    assert(output.state == UAV_STATE_RTL);
    assert(output.command == UAV_COMMAND_RETURN_HOME);
    assert(output.reason == UAV_REASON_GEOFENCE);
    assert(output.geofence_breached);
}

static void test_link_loss_timeout_enters_rtl(void) {
    uav_failsafe_controller_t controller;
    uav_input_t input;
    uav_output_t output = {0};
    unsigned i;

    uav_failsafe_controller_init(&controller);
    input.armed = true;
    input.rc_link_ok = false;
    input.gps_ok = true;
    input.gps_satellites = 10u;
    input.battery_pct = 68u;
    input.distance_home_m = 420u;
    input.altitude_m = 65u;
    input.geofence_radius_m = 900u;
    input.wind_dmps = 40u;
    for (i = 0u; i < 3u; ++i) {
        output = uav_failsafe_controller_step(&controller, &input);
    }

    assert(output.state == UAV_STATE_RTL);
    assert(output.reason == UAV_REASON_LINK_LOSS);
    assert(output.link_loss_count == 3u);
}

static void test_nav_loss_forces_landing(void) {
    uav_failsafe_controller_t controller;
    uav_input_t input;
    uav_output_t output = {0};
    unsigned i;

    uav_failsafe_controller_init(&controller);
    input.armed = true;
    input.rc_link_ok = false;
    input.gps_ok = false;
    input.gps_satellites = 0u;
    input.battery_pct = 61u;
    input.distance_home_m = 380u;
    input.altitude_m = 60u;
    input.geofence_radius_m = 900u;
    input.wind_dmps = 40u;
    for (i = 0u; i < 2u; ++i) {
        output = uav_failsafe_controller_step(&controller, &input);
    }

    assert(output.state == UAV_STATE_LAND);
    assert(output.command == UAV_COMMAND_DESCEND);
    assert(output.reason == UAV_REASON_NAV_LOSS);
}

static void test_touchdown_disarms_after_landing(void) {
    uav_failsafe_controller_t controller;
    uav_input_t landing_input;
    uav_input_t touchdown_input;
    uav_output_t output;

    uav_failsafe_controller_init(&controller);
    landing_input.armed = true;
    landing_input.rc_link_ok = true;
    landing_input.gps_ok = true;
    landing_input.gps_satellites = 9u;
    landing_input.battery_pct = 9u;
    landing_input.distance_home_m = 90u;
    landing_input.altitude_m = 18u;
    landing_input.geofence_radius_m = 500u;
    landing_input.wind_dmps = 20u;
    output = uav_failsafe_controller_step(&controller, &landing_input);
    assert(output.state == UAV_STATE_LAND);

    touchdown_input.armed = false;
    touchdown_input.rc_link_ok = true;
    touchdown_input.gps_ok = true;
    touchdown_input.gps_satellites = 9u;
    touchdown_input.battery_pct = 8u;
    touchdown_input.distance_home_m = 15u;
    touchdown_input.altitude_m = 0u;
    touchdown_input.geofence_radius_m = 500u;
    touchdown_input.wind_dmps = 20u;
    output = uav_failsafe_controller_step(&controller, &touchdown_input);

    assert(output.state == UAV_STATE_DISARMED);
    assert(output.command == UAV_COMMAND_CUT_MOTORS);
    assert(output.reason == UAV_REASON_NONE);
}

int main(void) {
    test_arming_block_when_gps_missing();
    test_geofence_breach_enters_rtl();
    test_link_loss_timeout_enters_rtl();
    test_nav_loss_forces_landing();
    test_touchdown_disarms_after_landing();
    return 0;
}
