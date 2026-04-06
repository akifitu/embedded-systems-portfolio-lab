#include <stdio.h>

#include "telemetry_node.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static int test_periodic_status_frame(void) {
    telemetry_node_t node;
    telemetry_sample_t sample = {48.0f, 5.0f, 30.0f, 80.0f, true};
    can_frame_t frames[4];

    telemetry_node_init(&node);
    ASSERT_TRUE(telemetry_node_step(&node, &sample, 100u, frames, 4u) == 1u);
    ASSERT_TRUE(frames[0].id == 0x180u);
    return 0;
}

static int test_fault_frame_priority(void) {
    telemetry_node_t node;
    telemetry_sample_t sample = {11.5f, 2.0f, 80.0f, 75.0f, false};
    can_frame_t frames[4];
    size_t count;

    telemetry_node_init(&node);
    count = telemetry_node_step(&node, &sample, 300u, frames, 4u);
    ASSERT_TRUE(count == 3u);
    ASSERT_TRUE(frames[0].id == 0x080u);
    ASSERT_TRUE(frames[1].id == 0x180u);
    ASSERT_TRUE(frames[2].id == 0x280u);
    return 0;
}

static int test_fault_bits_encoding(void) {
    telemetry_sample_t sample = {11.8f, 1.0f, 76.0f, 60.0f, false};
    uint32_t faults = telemetry_node_faults(&sample);

    ASSERT_TRUE((faults & NODE_FAULT_UNDERVOLTAGE) != 0u);
    ASSERT_TRUE((faults & NODE_FAULT_OVERTEMP) != 0u);
    ASSERT_TRUE((faults & NODE_FAULT_CONTACTOR_OPEN) != 0u);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_periodic_status_frame() == 0);
    ASSERT_TRUE(test_fault_frame_priority() == 0);
    ASSERT_TRUE(test_fault_bits_encoding() == 0);
    printf("can-telemetry-node tests passed\n");
    return 0;
}
