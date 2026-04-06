#include "ota_session.h"

#include <string.h>

#include "crc32.h"

void ota_session_begin(ota_session_t *session, semantic_version_t version,
                       size_t expected_size, uint32_t expected_crc32) {
    session->version = version;
    session->expected_size = expected_size;
    session->received_size = 0;
    session->expected_crc32 = expected_crc32;
    session->open = true;
}

bool ota_session_write_chunk(ota_session_t *session, const uint8_t *chunk,
                             size_t length) {
    if (!session->open) {
        return false;
    }

    if ((session->received_size + length) > OTA_MAX_IMAGE_SIZE ||
        (session->received_size + length) > session->expected_size) {
        return false;
    }

    memcpy(&session->buffer[session->received_size], chunk, length);
    session->received_size += length;
    return true;
}

bool ota_session_finalize(ota_session_t *session, firmware_image_t *image_out) {
    uint32_t actual_crc32;

    if (!session->open || session->received_size != session->expected_size) {
        return false;
    }

    actual_crc32 = crc32_compute(session->buffer, session->received_size);
    session->open = false;

    if (actual_crc32 != session->expected_crc32) {
        return false;
    }

    image_out->version = session->version;
    image_out->crc32 = actual_crc32;
    image_out->size_bytes = session->received_size;
    image_out->valid = true;
    image_out->confirmed = false;
    return true;
}

