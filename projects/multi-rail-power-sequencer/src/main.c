#include <stdio.h>
#include <string.h>

#include "power_sequencer.h"

static void clear_observation(pseq_observation_t *observation) {
    unsigned i;

    for (i = 0u; i < PSEQ_RAIL_COUNT; ++i) {
        observation->rail_pg[i] = false;
    }
    observation->brownout = false;
    observation->short_circuit = false;
}

static void print_snapshot(const char *phase, const pseq_output_t *output) {
    char fault_buffer[64];

    pseq_faults_to_string(output->faults, fault_buffer, sizeof(fault_buffer));
    printf(
        "phase=%s state=%s rail=%s mask=0x%X retries=%u progress=%u faults=%s\n",
        phase, pseq_state_name(output->state), pseq_rail_name(output->current_rail),
        output->enabled_mask, output->retry_count, output->progress_percent,
        fault_buffer);
}

static void run_nominal_sequence(void) {
    power_sequencer_t sequencer;
    pseq_command_t command = {0};
    pseq_observation_t observation;
    pseq_output_t output;
    int tick;

    power_sequencer_init(&sequencer);
    clear_observation(&observation);

    command.enable_request = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    print_snapshot("boot_start", &output);

    command.enable_request = false;
    observation.rail_pg[PSEQ_RAIL_CORE_1V0] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    print_snapshot("io_wait", &output);

    observation.rail_pg[PSEQ_RAIL_IO_1V8] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_AUX_3V3] = true;
    (void)power_sequencer_step(&sequencer, &command, &observation);
    observation.rail_pg[PSEQ_RAIL_RADIO_5V] = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    print_snapshot("stable", &output);

    command.shutdown_request = true;
    for (tick = 0; tick < 8; ++tick) {
        output = power_sequencer_step(&sequencer, &command, &observation);
        if (output.state == PSEQ_STATE_IDLE) {
            break;
        }
        command.shutdown_request = false;
    }
    print_snapshot("shutdown_complete", &output);
}

static void run_retry_and_fault_sequence(void) {
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
    print_snapshot("pg_timeout_retry", &output);

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
    print_snapshot("recovered", &output);

    observation.brownout = true;
    output = power_sequencer_step(&sequencer, &command, &observation);
    print_snapshot("brownout_fault", &output);
}

int main(void) {
    run_nominal_sequence();
    run_retry_and_fault_sequence();
    return 0;
}
