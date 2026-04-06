#ifndef CAN_FRAME_H
#define CAN_FRAME_H

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint32_t id;
    uint8_t dlc;
    uint8_t data[8];
} can_frame_t;

void can_frame_format(const can_frame_t *frame, char *buffer, size_t buffer_size);

#endif

