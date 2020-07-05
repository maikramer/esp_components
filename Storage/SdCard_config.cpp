//
// Created by maikeu on 31/12/2019.
//
#ifdef USE_SDCARD

#include "SdCard.h"

auto SdCard::StoreConfig(const std::string &key, const std::string &value, bool overwrite) -> StoreResult {
    const char *TAG = __FUNCTION__;

    ESP_LOGI(TAG, "Opening config file");

    auto res = StoreKeyValue(key, value, "config", overwrite);
    switch (res) {

        case StoreResult::Ok:
            ESP_LOGI(TAG, "Configuracao salva");
            break;
        case StoreResult::Exist:
            ESP_LOGW(TAG, "Configuracao ja existe");
            break;
        case StoreResult::Error:
            ESP_LOGE(TAG, "Erro salvando configuracao");
            break;
    }

    return res;
}

auto SdCard::LoadConfig(const std::string &key) -> std::string {
    const char *TAG = __FUNCTION__;

    ESP_LOGI(TAG, "Opening config file");
    auto ret = ReadKeyFromFile(key, "config.txt");

    if (ret.empty()) {
        ESP_LOGE(TAG, "Erro lendo a chave %s da configuracao", key.c_str());
    }

    return ret;
}

#endif