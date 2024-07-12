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
 * @brief Deployment data length
 */
#define MENDER_STORAGE_DEPLOYMENT_DATA_LENGTH (192)

/**
 * @brief Device configuration length
 */
#define MENDER_STORAGE_DEVICE_CONFIG_LENGTH (512)

/**
 * @brief Deployment data in SRAM
 */
__stm32_backup_sram_section char mender_storage_deployment_data[MENDER_STORAGE_DEPLOYMENT_DATA_LENGTH];

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
mender_storage_set_deployment_data(char *deployment_data) {

    assert(NULL != deployment_data);

    /* Retrieve length of the deployment data */
    size_t deployment_data_length = strlen(deployment_data);
    if (deployment_data_length >= MENDER_STORAGE_DEPLOYMENT_DATA_LENGTH) {
        mender_log_error("Unable to set deployment data");
        return MENDER_FAIL;
    }

    /* Copy deployment data */
    memcpy(mender_storage_deployment_data, deployment_data, deployment_data_length);
    memset(&mender_storage_deployment_data[deployment_data_length], '\0', MENDER_STORAGE_DEPLOYMENT_DATA_LENGTH - deployment_data_length);

#if __DCACHE_PRESENT
    /* Force cleaning the cache */
    SCB_CleanDCache_by_Addr(mender_storage_deployment_data, MENDER_STORAGE_DEPLOYMENT_DATA_LENGTH);
#endif

    return MENDER_OK;
}

mender_err_t
mender_storage_get_deployment_data(char **deployment_data) {

    assert(NULL != deployment_data);

    /* Retrieve length of the deployment data */
    if (0 == strlen(mender_storage_deployment_data)) {
        mender_log_info("Deployment data not available");
        return MENDER_NOT_FOUND;
    }

    /* Read deployment data */
    if (NULL == (*deployment_data = strdup(mender_storage_deployment_data))) {
        mender_log_error("Unable to read deployment data");
        return MENDER_FAIL;
    }

    return MENDER_OK;
}

mender_err_t
mender_storage_delete_deployment_data(void) {

    /* Reset deployment data */
    memset(mender_storage_deployment_data, 0, MENDER_STORAGE_DEPLOYMENT_DATA_LENGTH);

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
    memcpy(mender_storage_device_config, device_config, device_config_length);
    memset(&mender_storage_device_config[device_config_length], '\0', MENDER_STORAGE_DEVICE_CONFIG_LENGTH - device_config_length);

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
