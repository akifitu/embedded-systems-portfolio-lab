#include "boot_control.h"

#include <stdio.h>

semantic_version_t version_make(int major, int minor, int patch) {
    semantic_version_t version = {major, minor, patch};
    return version;
}

int version_compare(semantic_version_t left, semantic_version_t right) {
    if (left.major != right.major) {
        return (left.major > right.major) ? 1 : -1;
    }
    if (left.minor != right.minor) {
        return (left.minor > right.minor) ? 1 : -1;
    }
    if (left.patch != right.patch) {
        return (left.patch > right.patch) ? 1 : -1;
    }
    return 0;
}

void boot_control_init(boot_control_t *control, semantic_version_t version,
                       uint32_t crc32, size_t size_bytes) {
    control->primary.version = version;
    control->primary.crc32 = crc32;
    control->primary.size_bytes = size_bytes;
    control->primary.valid = true;
    control->primary.confirmed = true;
    control->secondary.valid = false;
    control->secondary.confirmed = false;
    control->secondary_pending = false;
    control->pending_mode = OTA_UPGRADE_NONE;
    control->trial_boot_active = false;
}

bool boot_control_stage_secondary(boot_control_t *control,
                                  const firmware_image_t *candidate) {
    if (!candidate->valid) {
        return false;
    }
    control->secondary = *candidate;
    control->secondary.confirmed = false;
    control->secondary_pending = false;
    control->pending_mode = OTA_UPGRADE_NONE;
    return true;
}

bool boot_control_request_upgrade(boot_control_t *control,
                                  ota_upgrade_mode_t mode) {
    if (!control->secondary.valid || mode == OTA_UPGRADE_NONE) {
        return false;
    }
    if (version_compare(control->secondary.version, control->primary.version) <=
        0) {
        return false;
    }

    control->secondary_pending = true;
    control->pending_mode = mode;
    return true;
}

firmware_image_t boot_control_reboot(boot_control_t *control) {
    if (control->trial_boot_active && !control->primary.confirmed &&
        control->secondary.valid) {
        firmware_image_t failed_image = control->primary;
        control->primary = control->secondary;
        control->secondary = failed_image;
        control->primary.confirmed = true;
        control->trial_boot_active = false;
        control->secondary_pending = false;
        control->pending_mode = OTA_UPGRADE_NONE;
        return control->primary;
    }

    if (control->secondary_pending && control->secondary.valid) {
        firmware_image_t previous_primary = control->primary;
        control->primary = control->secondary;
        control->secondary = previous_primary;
        control->primary.confirmed =
            (control->pending_mode == OTA_UPGRADE_PERMANENT);
        control->trial_boot_active =
            (control->pending_mode == OTA_UPGRADE_TEST);
        control->secondary_pending = false;
        control->pending_mode = OTA_UPGRADE_NONE;
    }

    return control->primary;
}

void boot_control_confirm_primary(boot_control_t *control) {
    control->primary.confirmed = true;
    control->trial_boot_active = false;
}

void boot_control_format_image(const firmware_image_t *image, char *buffer,
                               size_t buffer_size) {
    snprintf(buffer, buffer_size, "v%d.%d.%d crc=%08X size=%zu confirmed=%d",
             image->version.major, image->version.minor, image->version.patch,
             image->crc32, image->size_bytes, image->confirmed ? 1 : 0);
}

