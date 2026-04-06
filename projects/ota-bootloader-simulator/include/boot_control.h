#ifndef BOOT_CONTROL_H
#define BOOT_CONTROL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    int major;
    int minor;
    int patch;
} semantic_version_t;

typedef struct {
    semantic_version_t version;
    uint32_t crc32;
    size_t size_bytes;
    bool valid;
    bool confirmed;
} firmware_image_t;

typedef enum {
    OTA_UPGRADE_NONE = 0,
    OTA_UPGRADE_TEST,
    OTA_UPGRADE_PERMANENT
} ota_upgrade_mode_t;

typedef struct {
    firmware_image_t primary;
    firmware_image_t secondary;
    bool secondary_pending;
    ota_upgrade_mode_t pending_mode;
    bool trial_boot_active;
} boot_control_t;

semantic_version_t version_make(int major, int minor, int patch);
int version_compare(semantic_version_t left, semantic_version_t right);
void boot_control_init(boot_control_t *control, semantic_version_t version,
                       uint32_t crc32, size_t size_bytes);
bool boot_control_stage_secondary(boot_control_t *control,
                                  const firmware_image_t *candidate);
bool boot_control_request_upgrade(boot_control_t *control,
                                  ota_upgrade_mode_t mode);
firmware_image_t boot_control_reboot(boot_control_t *control);
void boot_control_confirm_primary(boot_control_t *control);
void boot_control_format_image(const firmware_image_t *image, char *buffer,
                               size_t buffer_size);

#endif

