#include <stdio.h>
#include <string.h>

#include "boot_control.h"
#include "crc32.h"
#include "ota_session.h"

#define ASSERT_TRUE(condition)                                                 \
    do {                                                                       \
        if (!(condition)) {                                                    \
            fprintf(stderr, "assertion failed: %s (%s:%d)\n", #condition,      \
                    __FILE__, __LINE__);                                       \
            return 1;                                                          \
        }                                                                      \
    } while (0)

static firmware_image_t build_image(const char *payload,
                                    semantic_version_t version) {
    ota_session_t session;
    firmware_image_t image = {0};
    size_t payload_size = strlen(payload);
    uint32_t crc = crc32_compute((const uint8_t *)payload, payload_size);

    ota_session_begin(&session, version, payload_size, crc);
    if (!ota_session_write_chunk(&session, (const uint8_t *)payload,
                                 payload_size)) {
        image.valid = false;
        return image;
    }
    if (!ota_session_finalize(&session, &image)) {
        image.valid = false;
        return image;
    }
    return image;
}

static int test_crc_mismatch_rejected(void) {
    const char *payload = "bad-crc-image";
    ota_session_t session;
    firmware_image_t image = {0};

    ota_session_begin(&session, version_make(1, 1, 0), strlen(payload),
                      0x12345678u);
    ASSERT_TRUE(ota_session_write_chunk(&session, (const uint8_t *)payload,
                                        strlen(payload)));
    ASSERT_TRUE(!ota_session_finalize(&session, &image));
    return 0;
}

static int test_test_upgrade_rolls_back(void) {
    boot_control_t control;
    firmware_image_t image =
        build_image("fw-1.1.0", version_make(1, 1, 0));

    boot_control_init(&control, version_make(1, 0, 0), 0xAAu, 64u);
    ASSERT_TRUE(boot_control_stage_secondary(&control, &image));
    ASSERT_TRUE(boot_control_request_upgrade(&control, OTA_UPGRADE_TEST));
    ASSERT_TRUE(boot_control_reboot(&control).version.minor == 1);
    ASSERT_TRUE(boot_control_reboot(&control).version.minor == 0);
    return 0;
}

static int test_permanent_upgrade_sticks(void) {
    boot_control_t control;
    firmware_image_t image =
        build_image("fw-1.2.0", version_make(1, 2, 0));

    boot_control_init(&control, version_make(1, 0, 0), 0xBBu, 64u);
    ASSERT_TRUE(boot_control_stage_secondary(&control, &image));
    ASSERT_TRUE(boot_control_request_upgrade(&control, OTA_UPGRADE_PERMANENT));
    ASSERT_TRUE(boot_control_reboot(&control).version.minor == 2);
    ASSERT_TRUE(boot_control_reboot(&control).version.minor == 2);
    return 0;
}

static int test_older_image_rejected(void) {
    boot_control_t control;
    firmware_image_t image =
        build_image("fw-0.9.0", version_make(0, 9, 0));

    boot_control_init(&control, version_make(1, 0, 0), 0xCCu, 64u);
    ASSERT_TRUE(boot_control_stage_secondary(&control, &image));
    ASSERT_TRUE(!boot_control_request_upgrade(&control, OTA_UPGRADE_TEST));
    return 0;
}

int main(void) {
    ASSERT_TRUE(test_crc_mismatch_rejected() == 0);
    ASSERT_TRUE(test_test_upgrade_rolls_back() == 0);
    ASSERT_TRUE(test_permanent_upgrade_sticks() == 0);
    ASSERT_TRUE(test_older_image_rejected() == 0);
    printf("ota-bootloader-simulator tests passed\n");
    return 0;
}
