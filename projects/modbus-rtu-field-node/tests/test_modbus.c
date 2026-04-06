#include <stdio.h>

#include "modbus_crc.h"
#include "modbus_rtu.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static void append_crc(uint8_t *frame, size_t *length) {
    uint16_t crc = mbn_crc16_compute(frame, *length);
    frame[*length] = (uint8_t)(crc & 0xFFu);
    frame[*length + 1u] = (uint8_t)((crc >> 8u) & 0xFFu);
    *length += 2u;
}

static int test_crc_reference(void) {
    uint8_t frame[] = {0x11u, 0x03u, 0x00u, 0x00u, 0x00u, 0x04u};
    uint16_t crc = mbn_crc16_compute(frame, sizeof(frame));

    ASSERT_TRUE(crc == 0x9946u);
    return 0;
}

static int test_read_holding_registers(void) {
    mbn_device_t device;
    uint8_t request[8] = {0x11u, 0x03u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x00u};
    uint8_t response[MBN_MAX_FRAME];
    size_t request_length = 6u;
    size_t response_length = 0u;

    append_crc(request, &request_length);
    mbn_device_init(&device, MBN_SLAVE_ID);
    ASSERT_TRUE(mbn_device_process_frame(&device, request, request_length, response,
                                         &response_length));
    ASSERT_TRUE(response[1] == 0x03u);
    ASSERT_TRUE(response[2] == 8u);
    ASSERT_TRUE(response[3] == 0x09u && response[4] == 0x29u);
    return 0;
}

static int test_write_single_register_updates_bank(void) {
    mbn_device_t device;
    uint8_t request[8] = {0x11u, 0x06u, 0x00u, 0x11u, 0x00u, 0x19u, 0x00u, 0x00u};
    uint8_t read_request[8] = {0x11u, 0x03u, 0x00u, 0x10u, 0x00u, 0x02u, 0x00u, 0x00u};
    uint8_t response[MBN_MAX_FRAME];
    size_t request_length = 6u;
    size_t read_length = 6u;
    size_t response_length = 0u;

    mbn_device_init(&device, MBN_SLAVE_ID);
    append_crc(request, &request_length);
    ASSERT_TRUE(mbn_device_process_frame(&device, request, request_length, response,
                                         &response_length));
    ASSERT_TRUE(response[1] == 0x06u);

    append_crc(read_request, &read_length);
    ASSERT_TRUE(mbn_device_process_frame(&device, read_request, read_length, response,
                                         &response_length));
    ASSERT_TRUE(response[3] == 0x00u && response[4] == 0x41u);
    ASSERT_TRUE(response[5] == 0x00u && response[6] == 0x19u);
    return 0;
}

static int test_illegal_address_exception(void) {
    mbn_device_t device;
    uint8_t request[8] = {0x11u, 0x03u, 0x00u, 0x20u, 0x00u, 0x02u, 0x00u, 0x00u};
    uint8_t response[MBN_MAX_FRAME];
    size_t request_length = 6u;
    size_t response_length = 0u;

    mbn_device_init(&device, MBN_SLAVE_ID);
    append_crc(request, &request_length);
    ASSERT_TRUE(mbn_device_process_frame(&device, request, request_length, response,
                                         &response_length));
    ASSERT_TRUE(response[1] == 0x83u);
    ASSERT_TRUE(response[2] == MBN_EXCEPTION_ILLEGAL_ADDRESS);
    return 0;
}

static int test_illegal_function_exception(void) {
    mbn_device_t device;
    uint8_t request[8] = {0x11u, 0x13u, 0x00u, 0x00u, 0x00u, 0x01u, 0x00u, 0x00u};
    uint8_t response[MBN_MAX_FRAME];
    size_t request_length = 6u;
    size_t response_length = 0u;

    mbn_device_init(&device, MBN_SLAVE_ID);
    append_crc(request, &request_length);
    ASSERT_TRUE(mbn_device_process_frame(&device, request, request_length, response,
                                         &response_length));
    ASSERT_TRUE(response[1] == 0x93u);
    ASSERT_TRUE(response[2] == MBN_EXCEPTION_ILLEGAL_FUNCTION);
    return 0;
}

static int test_bad_crc_is_rejected(void) {
    mbn_device_t device;
    uint8_t request[8] = {0x11u, 0x03u, 0x00u, 0x00u, 0x00u, 0x04u, 0x00u, 0x00u};
    uint8_t response[MBN_MAX_FRAME];
    size_t response_length = 0u;

    mbn_device_init(&device, MBN_SLAVE_ID);
    ASSERT_TRUE(!mbn_device_process_frame(&device, request, sizeof(request), response,
                                          &response_length));
    ASSERT_TRUE(response_length == 0u);
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_crc_reference() == 0);
    ASSERT_TRUE(test_read_holding_registers() == 0);
    ASSERT_TRUE(test_write_single_register_updates_bank() == 0);
    ASSERT_TRUE(test_illegal_address_exception() == 0);
    ASSERT_TRUE(test_illegal_function_exception() == 0);
    ASSERT_TRUE(test_bad_crc_is_rejected() == 0);
    printf("modbus-rtu-field-node tests passed\n");
    return 0;
}
