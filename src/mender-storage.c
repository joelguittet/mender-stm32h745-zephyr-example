/**
 * @file      mender-storage.c
 * @brief     Mender storage interface for project specific SRAM platform
 *
 * MIT License
 *
 * Copyright (c) 2022-2023 joelguittet and mender-mcu-client contributors
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include "mender-log.h"
#include "mender-storage.h"

/**
 * @brief Backup SRAM device
 */
#if DT_HAS_COMPAT_STATUS_OKAY(st_stm32_backup_sram)
#define BACKUP_DEV_COMPAT st_stm32_backup_sram
#endif

/**
 * @brief ID and artifact name length
 */
#define MENDER_STORAGE_OTA_ID_LENGTH            (64)
#define MENDER_STORAGE_OTA_ARTIFACT_NAME_LENGTH (64)

/**
 * @brief Device configuration length
 */
#define MENDER_STORAGE_DEVICE_CONFIG_LENGTH (512)

/**
 * @brief ID and artifact name in SRAM
 */
__stm32_backup_sram_section char mender_storage_ota_id[MENDER_STORAGE_OTA_ID_LENGTH];
__stm32_backup_sram_section char mender_storage_ota_artifact_name[MENDER_STORAGE_OTA_ARTIFACT_NAME_LENGTH];

/**
 * @brief Device configuration in SRAM
 */
__stm32_backup_sram_section char mender_storage_device_config[MENDER_STORAGE_DEVICE_CONFIG_LENGTH];

mender_err_t
mender_storage_init(void) {

    /* Check if backup SRAM device is ready */
    if (!device_is_ready(DEVICE_DT_GET_ONE(BACKUP_DEV_COMPAT))) {
        mender_log_error("Backup SRAM device is not ready");
        return MENDER_FAIL;
    }

    return MENDER_OK;
}

mender_err_t
mender_storage_set_ota_deployment(char *ota_id, char *ota_artifact_name) {

    assert(NULL != ota_id);
    assert(NULL != ota_artifact_name);

    /* Retrieve length of the ID and artifact name */
    size_t ota_id_length = strlen(ota_id);
    if (ota_id_length >= MENDER_STORAGE_OTA_ID_LENGTH) {
        mender_log_error("Unable to set OTA ID");
        return MENDER_FAIL;
    }
    size_t ota_artifact_name_length = strlen(ota_artifact_name);
    if (ota_artifact_name_length >= MENDER_STORAGE_OTA_ARTIFACT_NAME_LENGTH) {
        mender_log_error("Unable to set OTA artifact name");
        return MENDER_FAIL;
    }

    /* Copy ID and artifact name */
    memcpy(mender_storage_ota_id, ota_id, ota_id_length + 1);
    memcpy(mender_storage_ota_artifact_name, ota_artifact_name, ota_artifact_name_length + 1);

#if __DCACHE_PRESENT
    /* Force cleaning the cache */
    SCB_CleanDCache_by_Addr(mender_storage_ota_id, MENDER_STORAGE_OTA_ID_LENGTH);
    SCB_CleanDCache_by_Addr(mender_storage_ota_artifact_name, MENDER_STORAGE_OTA_ARTIFACT_NAME_LENGTH);
#endif

    return MENDER_OK;
}

mender_err_t
mender_storage_get_ota_deployment(char **ota_id, char **ota_artifact_name) {

    assert(NULL != ota_id);
    assert(NULL != ota_artifact_name);

    /* Retrieve length of the ID and artifact name */
    if (0 == strlen(mender_storage_ota_id)) {
        mender_log_info("OTA ID not available");
        return MENDER_NOT_FOUND;
    }
    if (0 == strlen(mender_storage_ota_artifact_name)) {
        mender_log_info("Artifact name not available");
        return MENDER_NOT_FOUND;
    }

    /* Read ID and artifact name */
    if (NULL == (*ota_id = strdup(mender_storage_ota_id))) {
        mender_log_error("Unable to read OTA ID");
        return MENDER_FAIL;
    }
    if (NULL == (*ota_artifact_name = strdup(mender_storage_ota_artifact_name))) {
        mender_log_error("Unable to read artifact name");
        free(*ota_id);
        *ota_id = NULL;
        return MENDER_FAIL;
    }

    return MENDER_OK;
}

mender_err_t
mender_storage_delete_ota_deployment(void) {

    /* Reset ID and artifact name */
    memset(mender_storage_ota_id, 0, MENDER_STORAGE_OTA_ID_LENGTH);
    memset(mender_storage_ota_artifact_name, 0, MENDER_STORAGE_OTA_ARTIFACT_NAME_LENGTH);

    return MENDER_OK;
}

#ifdef CONFIG_MENDER_CLIENT_ADD_ON_CONFIGURE
#ifdef CONFIG_MENDER_CLIENT_CONFIGURE_STORAGE

mender_err_t
mender_storage_set_device_config(char *device_config) {

    assert(NULL != device_config);

    /* Retrieve length of the device configuration */
    size_t device_config_length = strlen(device_config);
    if (device_config_length >= MENDER_STORAGE_DEVICE_CONFIG_LENGTH) {
        mender_log_error("Unable to set device configuration");
        return MENDER_FAIL;
    }

    /* Copy device configuration */
    memcpy(mender_storage_device_config, device_config, device_config_length + 1);

#if __DCACHE_PRESENT
    /* Force cleaning the cache */
    SCB_CleanDCache_by_Addr(mender_storage_device_config, MENDER_STORAGE_DEVICE_CONFIG_LENGTH);
#endif

    return MENDER_OK;
}

mender_err_t
mender_storage_get_device_config(char **device_config) {

    assert(NULL != device_config);

    /* Retrieve length of the device configuration */
    if (0 == strlen(mender_storage_device_config)) {
        mender_log_info("Device configuration not available");
        return MENDER_NOT_FOUND;
    }

    /* Read device configuration */
    if (NULL == (*device_config = strdup(mender_storage_device_config))) {
        mender_log_error("Unable to read device configuration");
        return MENDER_FAIL;
    }

    return MENDER_OK;
}

mender_err_t
mender_storage_delete_device_config(void) {

    /* Reset device configuration */
    memset(mender_storage_device_config, 0, MENDER_STORAGE_DEVICE_CONFIG_LENGTH);

    return MENDER_OK;
}

#endif /* CONFIG_MENDER_CLIENT_CONFIGURE_STORAGE */
#endif /* CONFIG_MENDER_CLIENT_ADD_ON_CONFIGURE */
