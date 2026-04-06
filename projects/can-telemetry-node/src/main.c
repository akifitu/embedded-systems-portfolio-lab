#include <stdio.h>

#include "telemetry_node.h"

int main(void) {
    telemetry_node_t node;
    telemetry_sample_t samples[] = {
        {48.2f, 6.2f, 31.5f, 74.0f, true},
        {47.9f, 7.0f, 32.0f, 73.6f, true},
        {11.2f, 3.0f, 79.0f, 72.4f, false},
        {11.0f, 2.0f, 70.0f, 72.0f, false},
        {48.0f, 4.0f, 34.0f, 71.7f, true},
    };
    can_frame_t frames[4];

    telemetry_node_init(&node);

    for (int i = 0; i < 5; ++i) {
        size_t count = telemetry_node_step(&node, &samples[i], 100u, frames, 4u);
        printf("tick=%d emitted=%zu\n", i, count);
        for (size_t j = 0; j < count; ++j) {
            char buffer[96];
            can_frame_format(&frames[j], buffer, sizeof(buffer));
            printf("  %s\n", buffer);
        }
    }

    return 0;
}

