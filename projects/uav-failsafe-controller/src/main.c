#include <stdio.h>

#include "failsafe_types.h"
#include "uav_failsafe_controller.h"

static void print_phase(const char *label, const uav_input_t *input,
                        const uav_output_t *output) {
    printf(
        "phase=%s state=%s cmd=%s reason=%s battery=%u reserve=%u budget=%u "
        "link=%u fence=%s health=%s\n",
        label, uav_state_name(output->state), uav_command_name(output->command),
        uav_reason_name(output->reason), input->battery_pct,
        output->reserve_margin_pct, output->return_budget_pct,
        output->link_loss_count, output->geofence_breached ? "OUT" : "IN",
        uav_health_name(output->health));
}

static uav_output_t run_steps(uav_failsafe_controller_t *controller,
                              const uav_input_t *input, unsigned steps) {
    uav_output_t output = {0};
    unsigned i;

    for (i = 0u; i < steps; ++i) {
        output = uav_failsafe_controller_step(controller, input);
    }
    return output;
}

static void demo_launch(void) {
    uav_failsafe_controller_t controller;
    uav_input_t input;
    uav_output_t output;

    uav_failsafe_controller_init(&controller);
    input.armed = true;
    input.rc_link_ok = true;
    input.gps_ok = true;
    input.gps_satellites = 12u;
    input.battery_pct = 92u;
    input.distance_home_m = 120u;
    input.altitude_m = 30u;
    input.geofence_radius_m = 600u;
    input.wind_dmps = 35u;
    output = run_steps(&controller, &input, 1u);
    print_phase("launch", &input, &output);
}

static void demo_geofence(void) {
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
    output = run_steps(&controller, &input, 1u);
    print_phase("geofence", &input, &output);
}

static void demo_link_loss(void) {
    uav_failsafe_controller_t controller;
    uav_input_t input;
    uav_output_t output;

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
    output = run_steps(&controller, &input, 3u);
    print_phase("link_loss", &input, &output);
}

static void demo_nav_loss(void) {
    uav_failsafe_controller_t controller;
    uav_input_t input;
    uav_output_t output;

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
    output = run_steps(&controller, &input, 2u);
    print_phase("nav_loss", &input, &output);
}

static void demo_critical_battery(void) {
    uav_failsafe_controller_t controller;
    uav_input_t input;
    uav_output_t output;

    uav_failsafe_controller_init(&controller);
    input.armed = true;
    input.rc_link_ok = true;
    input.gps_ok = true;
    input.gps_satellites = 9u;
    input.battery_pct = 9u;
    input.distance_home_m = 90u;
    input.altitude_m = 18u;
    input.geofence_radius_m = 500u;
    input.wind_dmps = 20u;
    output = run_steps(&controller, &input, 1u);
    print_phase("critical_battery", &input, &output);
}

static void demo_touchdown(void) {
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
    (void)run_steps(&controller, &landing_input, 1u);

    touchdown_input.armed = false;
    touchdown_input.rc_link_ok = true;
    touchdown_input.gps_ok = true;
    touchdown_input.gps_satellites = 9u;
    touchdown_input.battery_pct = 8u;
    touchdown_input.distance_home_m = 15u;
    touchdown_input.altitude_m = 0u;
    touchdown_input.geofence_radius_m = 500u;
    touchdown_input.wind_dmps = 20u;
    output = run_steps(&controller, &touchdown_input, 1u);
    print_phase("touchdown", &touchdown_input, &output);
}

int main(void) {
    demo_launch();
    demo_geofence();
    demo_link_loss();
    demo_nav_loss();
    demo_critical_battery();
    demo_touchdown();
    return 0;
}
