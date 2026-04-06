#include <assert.h>
#include <stdio.h>

#include "power_sequencer.h"

static void clear_observation(pseq_observation_t *observation) {
    unsigned i;

    for (i = 0u; i < PSEQ_RAIL_COUNT; ++i) {
        observation->rail_pg[i] = false;
    }
    observation->brownout = false;
    observation->short_circuit = false;
}

static void test_nominal_startup_reaches_stable(void) {
    power_sequencer_t sequencer;
    pseq_command_t command = {0};
    pseq_observation_t observation;
    pseq_output_t output;

    power_sequencer_init(&sequencer);
    clear_observation(&observation);

    command.enable_request = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    assert(output.state == PSEQ_STATE_STARTUP);
    assert(output.enabled_mask == 0x1u);

    command.enable_request = false;
    observation.rail_pg[PSEQ_RAIL_CORE_1V0] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    assert(output.current_rail == PSEQ_RAIL_IO_1V8);
    observation.rail_pg[PSEQ_RAIL_IO_1V8] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_AUX_3V3] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_RADIO_5V] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);

    assert(output.state == PSEQ_STATE_STABLE);
    assert(output.enabled_mask == 0xFu);
    assert(output.progress_percent == 100u);
}

static void test_pg_timeout_retry_then_recover(void) {
    power_sequencer_t sequencer;
    pseq_command_t command = {0};
    pseq_observation_t observation;
    pseq_output_t output = {0};
    int tick;

    power_sequencer_init(&sequencer);
    clear_observation(&observation);

    command.enable_request = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    command.enable_request = false;

    for (tick = 0; tick < 8; ++tick) {
        output = power_sequencer_step(&sequencer, &command, &observation);
        if (output.state == PSEQ_STATE_RETRY_WAIT) {
            break;
        }
    }

    assert(output.state == PSEQ_STATE_RETRY_WAIT);
    assert(output.retry_count == 1u);
    assert((output.faults & PSEQ_FAULT_PG_TIMEOUT) != 0u);

    while (output.state == PSEQ_STATE_RETRY_WAIT) {
        output = power_sequencer_step(&sequencer, &command, &observation);
    }

    observation.rail_pg[PSEQ_RAIL_CORE_1V0] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_IO_1V8] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_AUX_3V3] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_RADIO_5V] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);

    assert(output.state == PSEQ_STATE_STABLE);
    assert(output.retry_count == 1u);
    assert(output.faults == PSEQ_FAULT_NONE);
}

static void test_retry_exhaustion_faults(void) {
    power_sequencer_t sequencer;
    pseq_command_t command = {0};
    pseq_observation_t observation;
    pseq_output_t output = {0};
    int tick;

    power_sequencer_init(&sequencer);
    clear_observation(&observation);

    command.enable_request = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    command.enable_request = false;

    for (tick = 0; tick < 30; ++tick) {
        output = power_sequencer_step(&sequencer, &command, &observation);
        if (output.state == PSEQ_STATE_FAULT) {
            break;
        }
    }

    assert(output.state == PSEQ_STATE_FAULT);
    assert((output.faults & PSEQ_FAULT_PG_TIMEOUT) != 0u);
    assert((output.faults & PSEQ_FAULT_RETRY_EXHAUSTED) != 0u);
}

static void test_brownout_disables_rails(void) {
    power_sequencer_t sequencer;
    pseq_command_t command = {0};
    pseq_observation_t observation;
    pseq_output_t output;

    power_sequencer_init(&sequencer);
    clear_observation(&observation);

    command.enable_request = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    command.enable_request = false;
    observation.rail_pg[PSEQ_RAIL_CORE_1V0] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_IO_1V8] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_AUX_3V3] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_RADIO_5V] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);

    observation.brownout = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    assert(output.state == PSEQ_STATE_FAULT);
    assert(output.enabled_mask == 0u);
    assert((output.faults & PSEQ_FAULT_BROWNOUT) != 0u);
}

static void test_shutdown_returns_idle(void) {
    power_sequencer_t sequencer;
    pseq_command_t command = {0};
    pseq_observation_t observation;
    pseq_output_t output = {0};
    int tick;

    power_sequencer_init(&sequencer);
    clear_observation(&observation);

    command.enable_request = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    command.enable_request = false;
    observation.rail_pg[PSEQ_RAIL_CORE_1V0] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_IO_1V8] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_AUX_3V3] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_RADIO_5V] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);

    command.shutdown_request = true;
    for (tick = 0; tick < 8; ++tick) {
        output = power_sequencer_step(&sequencer, &command, &observation);
        command.shutdown_request = false;
        if (output.state == PSEQ_STATE_IDLE) {
            break;
        }
    }

    assert(output.state == PSEQ_STATE_IDLE);
    assert(output.enabled_mask == 0u);
}

int main(void) {
    test_nominal_startup_reaches_stable();
    test_pg_timeout_retry_then_recover();
    test_retry_exhaustion_faults();
    test_brownout_disables_rails();
    test_shutdown_returns_idle();

    puts("multi-rail-power-sequencer tests passed");
    return 0;
}
