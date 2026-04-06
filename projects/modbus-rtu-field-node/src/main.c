#include <stdio.h>

#include "modbus_crc.h"
#include "modbus_rtu.h"

static void append_crc_to_frame(uint8_t *frame, size_t *length) {
    uint16_t crc = mbn_crc16_compute(frame, *length);
    frame[*length] = (uint8_t)(crc & 0xFFu);
    frame[*length + 1u] = (uint8_t)((crc >> 8u) & 0xFFu);
    *length += 2u;
}

static void print_frame(const char *label, const uint8_t *frame, size_t length) {
    printf("%s", label);
    for (size_t i = 0; i < length; ++i) {
        printf("%s%02X", (i == 0u) ? " " : " ", frame[i]);
    }
    printf("\n");
}

static size_t make_read_request(uint8_t *frame, uint16_t address, uint16_t count) {
    size_t length = 6u;
    frame[0] = MBN_SLAVE_ID;
    frame[1] = 0x03u;
    frame[2] = (uint8_t)((address >> 8u) & 0xFFu);
    frame[3] = (uint8_t)(address & 0xFFu);
    frame[4] = (uint8_t)((count >> 8u) & 0xFFu);
    frame[5] = (uint8_t)(count & 0xFFu);
    append_crc_to_frame(frame, &length);
    return length;
}

static size_t make_write_request(uint8_t *frame, uint16_t address, uint16_t value) {
    size_t length = 6u;
    frame[0] = MBN_SLAVE_ID;
    frame[1] = 0x06u;
    frame[2] = (uint8_t)((address >> 8u) & 0xFFu);
    frame[3] = (uint8_t)(address & 0xFFu);
    frame[4] = (uint8_t)((value >> 8u) & 0xFFu);
    frame[5] = (uint8_t)(value & 0xFFu);
    append_crc_to_frame(frame, &length);
    return length;
}

static size_t make_illegal_function_request(uint8_t *frame) {
    size_t length = 6u;
    frame[0] = MBN_SLAVE_ID;
    frame[1] = 0x13u;
    frame[2] = 0x00u;
    frame[3] = 0x00u;
    frame[4] = 0x00u;
    frame[5] = 0x01u;
    append_crc_to_frame(frame, &length);
    return length;
}

int main(void) {
    mbn_device_t device;
    uint8_t request[MBN_MAX_FRAME];
    uint8_t response[MBN_MAX_FRAME];
    size_t request_length;
    size_t response_length;

    mbn_device_init(&device, MBN_SLAVE_ID);

    request_length = make_read_request(request, 0x0000u, 4u);
    mbn_device_process_frame(&device, request, request_length, response,
                             &response_length);
    print_frame("req1", request, request_length);
    print_frame("rsp1", response, response_length);

    request_length = make_write_request(request, 0x0011u, 0x0019u);
    mbn_device_process_frame(&device, request, request_length, response,
                             &response_length);
    print_frame("req2", request, request_length);
    print_frame("rsp2", response, response_length);

    request_length = make_read_request(request, 0x0010u, 2u);
    mbn_device_process_frame(&device, request, request_length, response,
                             &response_length);
    print_frame("req3", request, request_length);
    print_frame("rsp3", response, response_length);

    request_length = make_illegal_function_request(request);
    mbn_device_process_frame(&device, request, request_length, response,
                             &response_length);
    print_frame("rsp4", response, response_length);

    return 0;
}

