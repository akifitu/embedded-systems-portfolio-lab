#include "modbus_rtu.h"

#include "modbus_crc.h"

static void write_u16_be(uint8_t *buffer, size_t offset, uint16_t value) {
    buffer[offset] = (uint8_t)((value >> 8u) & 0xFFu);
    buffer[offset + 1u] = (uint8_t)(value & 0xFFu);
}

static uint16_t read_u16_be(const uint8_t *buffer, size_t offset) {
    return (uint16_t)(((uint16_t)buffer[offset] << 8u) | buffer[offset + 1u]);
}

static void append_crc(uint8_t *buffer, size_t *length) {
    uint16_t crc = mbn_crc16_compute(buffer, *length);
    buffer[*length] = (uint8_t)(crc & 0xFFu);
    buffer[*length + 1u] = (uint8_t)((crc >> 8u) & 0xFFu);
    *length += 2u;
}

static void build_exception(uint8_t slave_id, uint8_t function,
                            mbn_exception_t exception_code, uint8_t *response,
                            size_t *response_length) {
    response[0] = slave_id;
    response[1] = (uint8_t)(function | 0x80u);
    response[2] = (uint8_t)exception_code;
    *response_length = 3u;
    append_crc(response, response_length);
}

static bool validate_request_crc(const uint8_t *request, size_t length) {
    uint16_t expected;
    uint16_t actual;

    if (length < 4u) {
        return false;
    }

    actual = mbn_crc16_compute(request, length - 2u);
    expected = (uint16_t)(request[length - 2u] | ((uint16_t)request[length - 1u] << 8u));
    return actual == expected;
}

void mbn_device_init(mbn_device_t *device, uint8_t slave_id) {
    device->slave_id = slave_id;
    mbn_register_bank_init(&device->registers);
}

bool mbn_device_process_frame(mbn_device_t *device, const uint8_t *request,
                              size_t request_length, uint8_t *response_out,
                              size_t *response_length_out) {
    uint8_t function;

    *response_length_out = 0u;

    if (request_length < 4u || request[0] != device->slave_id ||
        !validate_request_crc(request, request_length)) {
        return false;
    }

    function = request[1];

    if (function == 0x03u) {
        uint16_t address = read_u16_be(request, 2u);
        uint16_t count = read_u16_be(request, 4u);
        uint16_t values[16];

        if (request_length != 8u || count == 0u || count > 16u) {
            build_exception(device->slave_id, function,
                            MBN_EXCEPTION_ILLEGAL_VALUE, response_out,
                            response_length_out);
            return true;
        }

        if (!mbn_register_bank_read(&device->registers, address, count, values)) {
            build_exception(device->slave_id, function,
                            MBN_EXCEPTION_ILLEGAL_ADDRESS, response_out,
                            response_length_out);
            return true;
        }

        response_out[0] = device->slave_id;
        response_out[1] = function;
        response_out[2] = (uint8_t)(count * 2u);
        *response_length_out = 3u;

        for (uint16_t i = 0u; i < count; ++i) {
            write_u16_be(response_out, *response_length_out, values[i]);
            *response_length_out += 2u;
        }
        append_crc(response_out, response_length_out);
        return true;
    }

    if (function == 0x06u) {
        uint16_t address = read_u16_be(request, 2u);
        uint16_t value = read_u16_be(request, 4u);

        if (request_length != 8u) {
            build_exception(device->slave_id, function,
                            MBN_EXCEPTION_ILLEGAL_VALUE, response_out,
                            response_length_out);
            return true;
        }

        if (!mbn_register_bank_write(&device->registers, address, value)) {
            build_exception(device->slave_id, function,
                            MBN_EXCEPTION_ILLEGAL_ADDRESS, response_out,
                            response_length_out);
            return true;
        }

        for (size_t i = 0u; i < 6u; ++i) {
            response_out[i] = request[i];
        }
        *response_length_out = 6u;
        append_crc(response_out, response_length_out);
        return true;
    }

    build_exception(device->slave_id, function, MBN_EXCEPTION_ILLEGAL_FUNCTION,
                    response_out, response_length_out);
    return true;
}

