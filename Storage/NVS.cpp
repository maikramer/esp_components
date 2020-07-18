//
// Created by maikeu on 15/08/2019.
//

#include "NVS.h"
#include <sstream>

static const char *PARTITION_NAME = "nvs";

void NVS::Init() {
    Storage::InitErrors();
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
}

auto NVS::LoadConfig(const std::string &key, std::string &config) -> ErrorCode {
    return ReadKeyValue<std::string>(key, config, "config");
}

auto NVS::StoreConfig(const std::string &key, std::string &value, bool overwrite) -> ErrorCode {
    return StoreKeyValue(key, value, "config", overwrite);
}

auto NVS::EraseData() -> bool {
    auto res = (nvs_flash_erase_partition(PARTITION_NAME) == ESP_OK);
    NVS::Init();
    return res;
}
