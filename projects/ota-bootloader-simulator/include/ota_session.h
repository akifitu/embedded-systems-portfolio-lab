#ifndef OTA_SESSION_H
#define OTA_SESSION_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "boot_control.h"

#define OTA_MAX_IMAGE_SIZE 1024

typedef struct {
    semantic_version_t version;
    uint8_t buffer[OTA_MAX_IMAGE_SIZE];
    size_t expected_size;
    size_t received_size;
    uint32_t expected_crc32;
    bool open;
} ota_session_t;

void ota_session_begin(ota_session_t *session, semantic_version_t version,
                       size_t expected_size, uint32_t expected_crc32);
bool ota_session_write_chunk(ota_session_t *session, const uint8_t *chunk,
                             size_t length);
bool ota_session_finalize(ota_session_t *session, firmware_image_t *image_out);

#endif

