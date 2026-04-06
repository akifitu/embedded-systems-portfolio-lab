#include <stdio.h>
#include <string.h>

#include "boot_control.h"
#include "crc32.h"
#include "ota_session.h"

static firmware_image_t receive_image(const char *payload,
                                      semantic_version_t version) {
    ota_session_t session;
    firmware_image_t image = {0};
    size_t payload_size = strlen(payload);
    uint32_t crc = crc32_compute((const uint8_t *)payload, payload_size);

    ota_session_begin(&session, version, payload_size, crc);
    ota_session_write_chunk(&session, (const uint8_t *)payload, 5);
    ota_session_write_chunk(&session, (const uint8_t *)payload + 5,
                            payload_size - 5);
    ota_session_finalize(&session, &image);
    return image;
}

static void print_active_image(const char *label, const firmware_image_t *image) {
    char buffer[96];
    boot_control_format_image(image, buffer, sizeof(buffer));
    printf("%s %s\n", label, buffer);
}

int main(void) {
    const char *payload_trial = "fw-1.1.0-power-profile";
    const char *payload_permanent = "fw-1.2.0-production-release";
    boot_control_t control;
    firmware_image_t active_image;
    firmware_image_t image;

    boot_control_init(
        &control, version_make(1, 0, 0),
        crc32_compute((const uint8_t *)"fw-1.0.0-factory",
                      strlen("fw-1.0.0-factory")),
        strlen("fw-1.0.0-factory"));
    print_active_image("factory:", &control.primary);

    image = receive_image(payload_trial, version_make(1, 1, 0));
    boot_control_stage_secondary(&control, &image);
    boot_control_request_upgrade(&control, OTA_UPGRADE_TEST);
    active_image = boot_control_reboot(&control);
    print_active_image("after test upgrade reboot:", &active_image);
    active_image = boot_control_reboot(&control);
    print_active_image("reboot without confirm:", &active_image);

    image = receive_image(payload_permanent, version_make(1, 2, 0));
    boot_control_stage_secondary(&control, &image);
    boot_control_request_upgrade(&control, OTA_UPGRADE_PERMANENT);
    active_image = boot_control_reboot(&control);
    print_active_image("after permanent upgrade reboot:", &active_image);
    active_image = boot_control_reboot(&control);
    print_active_image("final reboot:", &active_image);

    return 0;
}
