#include "WifiOta.h"
#include <esp_log.h>

const char* WifiOta::TAG = "WifiOta";

/**
 * @file WifiOta.cpp
 * @brief Implementation of the WifiOta class for handling OTA firmware updates over HTTPS.
 */

WifiOta::WifiOta() = default;

ErrorCode WifiOta::startUpdate(const std::string& url) {
    ESP_LOGI(TAG, "Starting OTA update from: %s", url.c_str());
    onUpdateStart.trigger();
    return performUpdate(url);
}

ErrorCode WifiOta::performUpdate(const std::string& url) {
    esp_http_client_config_t config = {};
    config.url = url.c_str();
    config.cert_pem = (char *)""; // Replace with your server certificate if needed

    esp_https_ota_config_t ota_config = {};
    ota_config.http_config = &config;
    ota_config.decrypt_cb = nullptr;
    ota_config.decrypt_user_ctx = nullptr;
    ota_config.enc_img_header_size = 0;

    esp_err_t err = esp_https_ota(&ota_config);
    if (err == ESP_OK) {
        ESP_LOGI(TAG, "OTA update successful!");
        onUpdateComplete.trigger();
        return CommonErrorCodes::None;
    } else {
        ESP_LOGE(TAG, "OTA update failed! Error: %s", esp_err_to_name(err));
        onUpdateFailed.trigger();
        // Return specific error codes based on the esp_https_ota error
        switch (err) {
            case ESP_ERR_INVALID_ARG:
                return CommonErrorCodes::ArgumentError;
            default:
                return CommonErrorCodes::OperationFailed; // General failure
        }
    }
}

esp_err_t WifiOta::otaProgressCallback(esp_https_ota_handle_t ota_handle, int progress, void* user_data) {
    auto* self = static_cast<WifiOta*>(user_data); // Get the WifiOta object
    self->onProgress.trigger(progress);
    return ESP_OK;
}