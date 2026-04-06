#include "telemetry_node.h"

static uint16_t encode_u16(float value, float scale) {
    if (value < 0.0f) {
        value = 0.0f;
    }
    return (uint16_t)(value * scale + 0.5f);
}

static int16_t encode_i16(float value, float scale) {
    if (value >= 0.0f) {
        return (int16_t)(value * scale + 0.5f);
    }
    return (int16_t)(value * scale - 0.5f);
}

static void sort_by_id(can_frame_t *frames, size_t count) {
    for (size_t i = 0; i < count; ++i) {
        for (size_t j = i + 1; j < count; ++j) {
            if (frames[j].id < frames[i].id) {
                can_frame_t temp = frames[i];
                frames[i] = frames[j];
                frames[j] = temp;
            }
        }
    }
}

uint32_t telemetry_node_faults(const telemetry_sample_t *sample) {
    uint32_t faults = NODE_FAULT_NONE;

    if (sample->pack_voltage_v < 12.0f) {
        faults |= NODE_FAULT_UNDERVOLTAGE;
    }
    if (sample->board_temp_c > 75.0f) {
        faults |= NODE_FAULT_OVERTEMP;
    }
    if (!sample->contactor_closed) {
        faults |= NODE_FAULT_CONTACTOR_OPEN;
    }

    return faults;
}

void telemetry_node_init(telemetry_node_t *node) {
    node->time_ms = 0;
    node->last_status_ms = 0;
    node->last_thermal_ms = 0;
    node->last_faults = NODE_FAULT_NONE;
}

size_t telemetry_node_step(telemetry_node_t *node,
                           const telemetry_sample_t *sample, uint32_t dt_ms,
                           can_frame_t *frames_out, size_t max_frames) {
    size_t frame_count = 0;
    uint32_t faults;

    node->time_ms += dt_ms;
    faults = telemetry_node_faults(sample);

    if (faults != node->last_faults && frame_count < max_frames) {
        can_frame_t fault_frame = {0};
        fault_frame.id = 0x080u;
        fault_frame.dlc = 2u;
        fault_frame.data[0] = (uint8_t)(faults & 0xFFu);
        fault_frame.data[1] = (uint8_t)((faults >> 8u) & 0xFFu);
        frames_out[frame_count++] = fault_frame;
        node->last_faults = faults;
    }

    if ((node->time_ms - node->last_status_ms) >= 100u &&
        frame_count < max_frames) {
        can_frame_t status_frame = {0};
        uint16_t voltage = encode_u16(sample->pack_voltage_v, 100.0f);
        int16_t current = encode_i16(sample->pack_current_a, 10.0f);
        uint16_t soc = encode_u16(sample->soc_percent, 10.0f);

        status_frame.id = 0x180u;
        status_frame.dlc = 6u;
        status_frame.data[0] = (uint8_t)(voltage & 0xFFu);
        status_frame.data[1] = (uint8_t)((voltage >> 8u) & 0xFFu);
        status_frame.data[2] = (uint8_t)(current & 0xFFu);
        status_frame.data[3] = (uint8_t)((current >> 8u) & 0xFFu);
        status_frame.data[4] = (uint8_t)(soc & 0xFFu);
        status_frame.data[5] = (uint8_t)((soc >> 8u) & 0xFFu);
        frames_out[frame_count++] = status_frame;
        node->last_status_ms = node->time_ms;
    }

    if ((node->time_ms - node->last_thermal_ms) >= 250u &&
        frame_count < max_frames) {
        can_frame_t thermal_frame = {0};
        uint16_t temp = encode_u16(sample->board_temp_c, 10.0f);

        thermal_frame.id = 0x280u;
        thermal_frame.dlc = 4u;
        thermal_frame.data[0] = (uint8_t)(temp & 0xFFu);
        thermal_frame.data[1] = (uint8_t)((temp >> 8u) & 0xFFu);
        thermal_frame.data[2] = (uint8_t)(faults & 0xFFu);
        thermal_frame.data[3] = sample->contactor_closed ? 1u : 0u;
        frames_out[frame_count++] = thermal_frame;
        node->last_thermal_ms = node->time_ms;
    }

    sort_by_id(frames_out, frame_count);
    return frame_count;
}

