#include "can_frame.h"

#include <stdio.h>

void can_frame_format(const can_frame_t *frame, char *buffer, size_t buffer_size) {
    snprintf(buffer, buffer_size,
             "vcan0 %03X [%u] %02X %02X %02X %02X %02X %02X %02X %02X",
             frame->id, frame->dlc, frame->data[0], frame->data[1],
             frame->data[2], frame->data[3], frame->data[4], frame->data[5],
             frame->data[6], frame->data[7]);
}

