#include "NVS.h"
#include <esp_log.h>

/**
 * @file NVS.cpp
 * @brief Implementation of the NVS class for non-volatile storage operations.
 */

ErrorCode NVS::initialize() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "NVS initialization failed: %s", esp_err_to_name(err));
        return CommonErrorCodes::StorageInitFailed;
    }
    return CommonErrorCodes::None;
}

ErrorCode NVS::eraseData() {
    esp_err_t err = nvs_flash_erase_partition(NVSConstants::PartitionName);
    if (err != ESP_OK) {
        ESP_LOGE("NVS", "Failed to erase NVS partition: %s", esp_err_to_name(err));
        return CommonErrorCodes::OperationFailed;
    }
    // Re-initialize NVS after erasing 
    return initialize();
}