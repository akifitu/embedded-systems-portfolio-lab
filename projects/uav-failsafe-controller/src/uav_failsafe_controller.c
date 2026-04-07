#include "uav_failsafe_controller.h"

#include <stdbool.h>
#include <stdint.h>

#include "geofence.h"
#include "reserve_model.h"

#define UAV_MIN_ARM_BATTERY_PCT 25u
#define UAV_MIN_ARM_SATELLITES 6u
#define UAV_LINK_LOSS_THRESHOLD 3u
#define UAV_NAV_LOSS_THRESHOLD 2u
#define UAV_CRITICAL_BATTERY_PCT 10u
#define UAV_RTL_REENTRY_DISTANCE_M 50u

static uint8_t saturating_increment(uint8_t value) {
    return (value == UINT8_MAX) ? value : (uint8_t)(value + 1u);
}

static bool arming_checks_pass(const uav_input_t *input) {
    return input->gps_ok && (input->gps_satellites >= UAV_MIN_ARM_SATELLITES) &&
           (input->battery_pct >= UAV_MIN_ARM_BATTERY_PCT);
}

static uav_health_t classify_health(uav_state_t state, uav_reason_t reason,
                                    uint8_t reserve_margin_pct,
                                    uint8_t link_loss_count,
                                    bool geofence_breached,
                                    uint8_t battery_pct) {
    if ((state == UAV_STATE_LAND) || (reason == UAV_REASON_NAV_LOSS) ||
        (reason == UAV_REASON_CRITICAL_BATTERY) ||
        ((state == UAV_STATE_DISARMED) && (battery_pct <= UAV_CRITICAL_BATTERY_PCT))) {
        return UAV_HEALTH_RED;
    }
    if ((state == UAV_STATE_RTL) || geofence_breached || (link_loss_count > 0u) ||
        (reserve_margin_pct < 25u) || (reason == UAV_REASON_ARMING_BLOCK)) {
        return UAV_HEALTH_YELLOW;
    }
    return UAV_HEALTH_GREEN;
}

static uav_output_t make_output(uav_failsafe_controller_t *controller,
                                const reserve_estimate_t *reserve,
                                const geofence_status_t *geofence,
                                const uav_input_t *input, uav_command_t command,
                                uav_reason_t reason) {
    uav_output_t output;

    output.state = controller->state;
    output.command = command;
    output.reason = reason;
    output.reserve_margin_pct = reserve->reserve_margin_pct;
    output.return_budget_pct = reserve->return_budget_pct;
    output.link_loss_count = controller->link_loss_count;
    output.geofence_breached = geofence->breached;
    output.health = classify_health(output.state, output.reason,
                                    output.reserve_margin_pct,
                                    output.link_loss_count,
                                    output.geofence_breached,
                                    input->battery_pct);
    return output;
}

void uav_failsafe_controller_init(uav_failsafe_controller_t *controller) {
    controller->state = UAV_STATE_STANDBY;
    controller->link_loss_count = 0u;
    controller->nav_loss_count = 0u;
    controller->latched_reason = UAV_REASON_NONE;
}

uav_output_t uav_failsafe_controller_step(uav_failsafe_controller_t *controller,
                                          const uav_input_t *input) {
    reserve_estimate_t reserve;
    geofence_status_t geofence;

    reserve = reserve_model_estimate(input);
    geofence = geofence_evaluate(input->distance_home_m, input->geofence_radius_m);

    if (input->armed) {
        controller->link_loss_count =
            input->rc_link_ok ? 0u
                              : saturating_increment(controller->link_loss_count);
        controller->nav_loss_count =
            input->gps_ok ? 0u
                          : saturating_increment(controller->nav_loss_count);
    } else {
        controller->link_loss_count = 0u;
        controller->nav_loss_count = 0u;
    }

    if (!input->armed) {
        if ((controller->state == UAV_STATE_LAND) ||
            (controller->state == UAV_STATE_DISARMED)) {
            controller->state = UAV_STATE_DISARMED;
            controller->latched_reason = UAV_REASON_NONE;
            return make_output(controller, &reserve, &geofence, input,
                               UAV_COMMAND_CUT_MOTORS, UAV_REASON_NONE);
        }

        controller->state = UAV_STATE_STANDBY;
        controller->latched_reason =
            arming_checks_pass(input) ? UAV_REASON_NONE : UAV_REASON_ARMING_BLOCK;
        return make_output(controller, &reserve, &geofence, input, UAV_COMMAND_HOLD,
                           controller->latched_reason);
    }

    if ((controller->state == UAV_STATE_LAND) && (input->altitude_m == 0u)) {
        controller->state = UAV_STATE_DISARMED;
        controller->latched_reason = UAV_REASON_NONE;
        return make_output(controller, &reserve, &geofence, input,
                           UAV_COMMAND_CUT_MOTORS, UAV_REASON_NONE);
    }

    if ((input->battery_pct <= UAV_CRITICAL_BATTERY_PCT) ||
        ((controller->state == UAV_STATE_LAND) && (input->altitude_m > 0u))) {
        controller->state = UAV_STATE_LAND;
        controller->latched_reason = UAV_REASON_CRITICAL_BATTERY;
        return make_output(controller, &reserve, &geofence, input,
                           UAV_COMMAND_DESCEND, controller->latched_reason);
    }

    if ((!input->gps_ok) &&
        (controller->nav_loss_count >= UAV_NAV_LOSS_THRESHOLD) &&
        (controller->link_loss_count >= 1u)) {
        controller->state = UAV_STATE_LAND;
        controller->latched_reason = UAV_REASON_NAV_LOSS;
        return make_output(controller, &reserve, &geofence, input,
                           UAV_COMMAND_DESCEND, controller->latched_reason);
    }

    if (geofence.breached) {
        controller->state = UAV_STATE_RTL;
        controller->latched_reason = UAV_REASON_GEOFENCE;
        return make_output(controller, &reserve, &geofence, input,
                           UAV_COMMAND_RETURN_HOME, controller->latched_reason);
    }

    if (controller->link_loss_count >= UAV_LINK_LOSS_THRESHOLD) {
        controller->state = input->gps_ok ? UAV_STATE_RTL : UAV_STATE_LAND;
        controller->latched_reason =
            input->gps_ok ? UAV_REASON_LINK_LOSS : UAV_REASON_NAV_LOSS;
        return make_output(controller, &reserve, &geofence, input,
                           input->gps_ok ? UAV_COMMAND_RETURN_HOME
                                         : UAV_COMMAND_DESCEND,
                           controller->latched_reason);
    }

    if (!reserve.reserve_safe) {
        controller->state = input->gps_ok ? UAV_STATE_RTL : UAV_STATE_LAND;
        controller->latched_reason =
            input->gps_ok ? UAV_REASON_RETURN_MARGIN : UAV_REASON_NAV_LOSS;
        return make_output(controller, &reserve, &geofence, input,
                           input->gps_ok ? UAV_COMMAND_RETURN_HOME
                                         : UAV_COMMAND_DESCEND,
                           controller->latched_reason);
    }

    if ((controller->state == UAV_STATE_RTL) &&
        (input->distance_home_m <= UAV_RTL_REENTRY_DISTANCE_M) &&
        input->rc_link_ok && input->gps_ok) {
        controller->state = UAV_STATE_MISSION;
        controller->latched_reason = UAV_REASON_NONE;
        return make_output(controller, &reserve, &geofence, input,
                           UAV_COMMAND_PROCEED, UAV_REASON_NONE);
    }

    controller->state = UAV_STATE_MISSION;
    controller->latched_reason = UAV_REASON_NONE;
    return make_output(controller, &reserve, &geofence, input, UAV_COMMAND_PROCEED,
                       UAV_REASON_NONE);
}
