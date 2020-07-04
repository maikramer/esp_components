//
// Created by maikeu on 15/08/2019.
//

#include <nvs_flash.h>
#include <esp_err.h>
#include <esp_log.h>
#include "Flash.h"
#include <sstream>

static const char *PARTITION_NAME = "nvs";

void Flash::Init() {
    // Initialize NVS
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    ESP_ERROR_CHECK(err);
}

auto Flash::StoreKeyValue(const std::string &key, std::string &str, const std::string &fileName,
                          bool overwrite) -> StoreResult {
    return StoreKeyValue(key, reinterpret_cast<void *>(&str), fileName, DataType::String, overwrite);
}

auto Flash::StoreKeyValue(const std::string &key, uint32_t value, const std::string &fileName,
                          bool overwrite) -> StoreResult {
    return StoreKeyValue(key, reinterpret_cast<void *>(&value), fileName, DataType::Uint32, overwrite);
}

auto Flash::StoreKeyValue(const std::string &key, int32_t value, const std::string &fileName,
                          bool overwrite) -> StoreResult {
    return StoreKeyValue(key, reinterpret_cast<void *>(&value), fileName, DataType::Int32, overwrite);
}

auto Flash::StoreKeyValue(const std::string &key, void *value, const std::string &fileName, DataType type,
                          bool overwrite) -> StoreResult {
    const char *TAG = __FUNCTION__;
    StoreResult result;
    error_t err = 0;
    char outputStr[256];
    int64_t signedOut = 0;
    uint64_t unsignedOut = 0;
    size_t outputSize = 0;
    nvs_handle_t file_handle = 0;

    if (type == DataType::String) {
        auto *casted = static_cast<std::string *>(value);
        if (casted->length() > 256) {
            ESP_LOGE(__FUNCTION__, "Valor com mais de 256 caracteres");
            result = StoreResult::Error;
            goto end;
        }
    }


    ESP_LOGI(TAG, "Abrindo %s", fileName.c_str());
    err = nvs_open_from_partition(PARTITION_NAME, fileName.c_str(), NVS_READWRITE, &file_handle);
    if (err != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Erro (%s) abrindo NVS handle", esp_err_to_name(err));
        result = StoreResult::Error;
        goto end;
    }

    if (!overwrite) {
        switch (type) {
            case DataType::String:
                err = nvs_get_str(file_handle, key.c_str(), outputStr, &outputSize);
                break;
            case DataType::Int64:
                err = nvs_get_i64(file_handle, key.c_str(), &signedOut);
                break;
            case DataType::Uint64:
                err = nvs_get_u64(file_handle, key.c_str(), &unsignedOut);
                break;
            case DataType::Int32:
                err = nvs_get_i32(file_handle, key.c_str(), reinterpret_cast<int32_t *>(&signedOut));
                break;
            case DataType::Uint32:
                err = nvs_get_u32(file_handle, key.c_str(), reinterpret_cast<uint32_t *>(&unsignedOut));
                break;
            case DataType::Int8:
                err = nvs_get_i8(file_handle, key.c_str(), reinterpret_cast<int8_t *>(&signedOut));
                break;
            case DataType::Uint8:
                err = nvs_get_u8(file_handle, key.c_str(), reinterpret_cast<uint8_t *>(&unsignedOut));
                break;
            default:
                ESP_LOGE(__FUNCTION__, "Tipo de dados invalido");
                result = StoreResult::Error;
                goto end;

        }

        if (err == ESP_OK) {
            ESP_LOGE(__FUNCTION__, "Chave existe");
            return StoreResult::Exist;
        } else if (err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(__FUNCTION__, "Erro (%s) na busca pela chave", esp_err_to_name(err));
            result = StoreResult::Error;
            goto end;
        }
    }

    ESP_LOGI(__FUNCTION__, "Gravando chave");
    switch (type) {
        case DataType::String:
            err = nvs_set_str(file_handle, key.c_str(), static_cast<std::string *>(value)->c_str());
            break;
        case DataType::Int64:
            err = nvs_set_i64(file_handle, key.c_str(), *(static_cast<int64_t *>(value)));
            break;
        case DataType::Uint64:
            err = nvs_set_u64(file_handle, key.c_str(), *(static_cast<uint64_t *>(value)));
            break;
        case DataType::Int32:
            err = nvs_set_i32(file_handle, key.c_str(), *(static_cast<int32_t *>(value)));
            break;
        case DataType::Uint32:
            err = nvs_set_u32(file_handle, key.c_str(), *(static_cast<int32_t *>(value)));
            break;
        case DataType::Int8:
            err = nvs_set_i8(file_handle, key.c_str(), *(static_cast<int8_t *>(value)));
            break;
        case DataType::Uint8:
            err = nvs_set_u8(file_handle, key.c_str(), *(static_cast<int8_t *>(value)));
            break;
        default:
            ESP_LOGE(__FUNCTION__, "Tipo de dados invalido");
            result = StoreResult::Error;
            goto end;
    }

    if (err != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Erro (%s) gravando dado", esp_err_to_name(err));
        result = StoreResult::Error;
        goto end;
    } else {
        nvs_commit(file_handle);
        ESP_LOGI(__FUNCTION__, "Sucesso salvando chave/valor");
        result = StoreResult::Ok;
    }

    end:
    nvs_close(file_handle);
    return result;
}

auto
Flash::ReadKeyValue(const std::string &key, uint32_t * out, const std::string &fileName) -> StoreResult {
    StoreResult result = ReadKeyValue(key, out, fileName, DataType::Uint32);
    return result;
}

auto
Flash::ReadKeyValue(const std::string &key, std::string &out, const std::string &fileName) -> StoreResult {
    char outChars[256];
    StoreResult result = ReadKeyValue(key, &outChars, fileName, DataType::String);
    out = outChars;
    return result;
}

auto
Flash::ReadKeyValue(const std::string &key, void *out, const std::string &fileName, DataType type) -> StoreResult {
    const char *TAG = __FUNCTION__;
    StoreResult result;
    error_t err = 0;
    size_t outputSize = 0;
    nvs_handle_t file_handle = 0;


    ESP_LOGI(TAG, "Abrindo %s", fileName.c_str());
    err = nvs_open_from_partition(PARTITION_NAME, fileName.c_str(), NVS_READWRITE, &file_handle);
    if (err != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Erro (%s) abrindo NVS handle", esp_err_to_name(err));
        result = StoreResult::Error;
        goto end;
    }

    switch (type) {
        case DataType::String:
            err = nvs_get_str(file_handle, key.c_str(), reinterpret_cast<char *>(out), &outputSize);
            break;
        case DataType::Int64:
            err = nvs_get_i64(file_handle, key.c_str(), reinterpret_cast<int64_t *>(out));
            break;
        case DataType::Uint64:
            err = nvs_get_u64(file_handle, key.c_str(), reinterpret_cast<uint64_t *>(out));
            break;
        case DataType::Int32:
            err = nvs_get_i32(file_handle, key.c_str(), reinterpret_cast<int32_t *>(out));
            break;
        case DataType::Uint32:
            err = nvs_get_u32(file_handle, key.c_str(), reinterpret_cast<uint32_t *>(out));
            break;
        case DataType::Int8:
            err = nvs_get_i8(file_handle, key.c_str(), reinterpret_cast<int8_t *>(out));
            break;
        case DataType::Uint8:
            err = nvs_get_u8(file_handle, key.c_str(), reinterpret_cast<uint8_t *>(out));
            break;
        default:
            ESP_LOGE(__FUNCTION__, "Tipo de dados invalido");
            result = StoreResult::Error;
            goto end;
    }

    if (err != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Erro (%s) lendo dados", esp_err_to_name(err));
        result = StoreResult::Error;
        goto end;
    } else {
        nvs_commit(file_handle);
        ESP_LOGI(__FUNCTION__, "Sucesso salvando chave/valor");
        result = StoreResult::Ok;
    }

    end:
    nvs_close(file_handle);
    return result;
}

auto Flash::LoadConfig(const std::string &key, std::string &config) -> StoreResult {
    return ReadKeyValue(key, config, "config");
}

auto Flash::StoreConfig(const std::string &key, std::string &value, bool overwrite) -> StoreResult {
    return StoreKeyValue(key, value, "config", overwrite);
}
