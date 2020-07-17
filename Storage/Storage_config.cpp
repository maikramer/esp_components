//
// Created by maikeu on 31/12/2019.
//

#include "Storage.h"

auto Storage::StoreConfig(const std::string &key, const std::string &value, bool overwrite) -> ErrorCode {
    const char *TAG = __FUNCTION__;

    ESP_LOGI(TAG, "Opening config file");

    auto res = StoreKeyValue(key, value, "config", overwrite);
    switch ((uint8_t) res) {

        case ErrorCodes::None:
            ESP_LOGI(TAG, "Configuracao salva");
            break;
        case ErrorCodes::Exist:
            ESP_LOGW(TAG, "Configuracao ja existe");
            break;
        case ErrorCodes::FileNotFound:
        case ErrorCodes::KeyNotFound:
        case ErrorCodes::FindError:
        case ErrorCodes::Error:
            ESP_LOGE(TAG, "Erro salvando configuracao");
            break;
    }

    return res;
}

auto Storage::LoadConfig(const std::string &key) -> std::string {
    const char *TAG = __FUNCTION__;

    ESP_LOGI(TAG, "Opening config file");
    std::string ret;
    auto result = ReadKeyFromFile(key, ret, "config.txt");

    if (result != ErrorCodes::None || ret.empty()) {
        ESP_LOGE(TAG, "Erro lendo a chave %s da configuracao", key.c_str());
    }

    return ret;
}
