//
// Created by maikeu on 15/08/2019.
//

#ifndef NVS_H
#define NVS_H

#include <map>
#include <list>
#include <string>
#include "Storage.h"
#include <nvs_flash.h>
#include <esp_err.h>
#include <esp_log.h>

namespace FlashConsts {
    static const char *PARTITION_NAME = "nvs";
}


class NVS {
public:
    static void Init();

    static auto
    StoreConfig(const std::string &key, std::string &value, bool overwrite) -> ErrorCode;

    static auto LoadConfig(const std::string &key, std::string &config) -> ErrorCode;

    template<typename T>
    static auto GetAllKeysFromFile(const std::string &filename,
                                   std::map<std::string, T> &valueMap) -> ErrorCode;


    template<typename T>
    static auto
    StoreKeyValue(const std::string &key, T value, const std::string &fileName,
                  bool overwrite) -> ErrorCode;

    template<typename T>
    static auto
    ReadKeyValue(const std::string &key, T &out, const std::string &fileName) -> ErrorCode;

    static auto EraseData() -> bool;

private:


};

template<typename T>
auto NVS::GetAllKeysFromFile(const std::string &filename,
                             std::map<std::string, T> &valueMap) -> ErrorCode {
    nvs_type_t type;//NOLINT
    if (std::is_same<T, std::string>()) {
        type = NVS_TYPE_STR;
    } else if (std::is_same<T, std::int32_t>()) {
        type = NVS_TYPE_I32;
    } else if (std::is_same<T, std::uint32_t>()) {
        type = NVS_TYPE_U32;
    } else if (std::is_same<T, std::int64_t>()) {
        type = NVS_TYPE_I64;
    } else if (std::is_same<T, std::uint64_t>()) {
        type = NVS_TYPE_U64;
    } else {
        ESP_LOGE(__FUNCTION__, "Tipo invalido ou nao suportado");
        return ErrorCode(ErrorCodes::Error);
    }

    nvs_iterator_t it;
    auto res = nvs_entry_find(FlashConsts::PARTITION_NAME, filename.c_str(), type, &it);

    if (res != ESP_OK || it == nullptr) {
        ESP_LOGW(__FUNCTION__, "Erro buscando chave, Codigo: %s",
                 esp_err_to_name(res));
        return ErrorCode(ErrorCodes::FindError);
    }

    while (it != nullptr) {
        nvs_entry_info_t info;
        nvs_entry_info(it, &info);
        T value;
        std::string key = info.key;
        auto result = ReadKeyValue(key, value, filename);

        if (result != ErrorCodes::None) {
            ESP_LOGE(__FUNCTION__, "Erro lendo chaves e valores do arquivo");
            return result;
        }

        valueMap.emplace(key, value);
        res = nvs_entry_next(&it);
        if (res != ESP_OK) {
            ESP_LOGW(__FUNCTION__, "Erro buscando proxima chave, Codigo: %s",
                     esp_err_to_name(res));
            return result;
        }
    }
    ESP_LOGI(__FUNCTION__, "%d valores lidos", valueMap.size());

    return ErrorCode(ErrorCodes::None);
}

#endif //NVS_H


template<typename T>
auto NVS::StoreKeyValue(const std::string &key, T value, const std::string &fileName,
                        bool overwrite) -> ErrorCode {
    ErrorCode result = ErrorCodes::None;
    error_t err;//NOLINT

    char outputStr[256];
    T integerValue;
    nvs_handle_t file_handle = 0;
    size_t outputSize = 0;

#ifdef LOG
    ESP_LOGI(TAG, "Abrindo %s", fileName.c_str());
#endif
    err = nvs_open_from_partition(FlashConsts::PARTITION_NAME, fileName.c_str(), NVS_READWRITE,
                                  &file_handle);
    if (err != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Erro (%s) abrindo NVS handle", esp_err_to_name(err));
        return ErrorCode(ErrorCodes::FileNotFound);
    }

    if (!overwrite) {
        if (std::is_same<T, std::string>()) {
            err = nvs_get_str(file_handle, key.c_str(), outputStr, &outputSize);
        } else if (std::is_same<T, std::int32_t>()) {
            err = nvs_get_i32(file_handle, key.c_str(), reinterpret_cast<int32_t *>(&integerValue));
        } else if (std::is_same<T, std::uint32_t>()) {
            err = nvs_get_u32(file_handle, key.c_str(),
                              reinterpret_cast<uint32_t *>(&integerValue));
        } else if (std::is_same<T, std::int64_t>()) {
            err = nvs_get_i64(file_handle, key.c_str(), reinterpret_cast<int64_t *>(&integerValue));
        } else if (std::is_same<T, std::uint64_t>()) {
            err = nvs_get_u64(file_handle, key.c_str(),
                              reinterpret_cast<uint64_t *>(&integerValue));
        } else {
            ESP_LOGE(__FUNCTION__, "Tipo invalido ou nao suportado");
        }
        if (err == ESP_OK) {
            ESP_LOGE(__FUNCTION__, "Chave existe");
            return ErrorCode(ErrorCodes::Exist);
        } else if (err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(__FUNCTION__, "Erro (%s) na busca pela chave", esp_err_to_name(err));
            result = ErrorCode(ErrorCodes::Error);
            goto end;
        }
    }
#ifdef LOG
    ESP_LOGI(__FUNCTION__, "Gravando chave");
#endif
    if (std::is_same<T, std::string>()) {
        err = nvs_set_str(file_handle, key.c_str(),
                          reinterpret_cast<std::string *>(&value)->c_str());
    } else if (std::is_same<T, std::int32_t>()) {
        err = nvs_set_i32(file_handle, key.c_str(), *reinterpret_cast<int32_t *>(&value));
    } else if (std::is_same<T, std::uint32_t>()) {
        err = nvs_set_u32(file_handle, key.c_str(), *reinterpret_cast<uint32_t *>(&value));
    } else if (std::is_same<T, std::int64_t>()) {
        err = nvs_set_i64(file_handle, key.c_str(), *reinterpret_cast<int64_t *>(&value));
    } else if (std::is_same<T, std::uint64_t>()) {
        err = nvs_set_u64(file_handle, key.c_str(), *reinterpret_cast<uint64_t *>(&value));
    } else {
        ESP_LOGE(__FUNCTION__, "Tipo invalido ou nao suportado");
    }

    if (err != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Erro (%s) gravando dado", esp_err_to_name(err));
        result = ErrorCode(ErrorCodes::Error);
        goto end;
    } else {
        nvs_commit(file_handle);
#ifdef LOG
        ESP_LOGI(__FUNCTION__, "Sucesso salvando chave/valor");
#endif
        result = ErrorCode(ErrorCodes::None);
    }

    end:
    nvs_close(file_handle);
    return result;
}


template<typename T>
auto
NVS::ReadKeyValue(const std::string &key, T &out, const std::string &fileName) -> ErrorCode {
    ErrorCode result = ErrorCodes::None;
    error_t err;//NOLINT
    size_t outputSize = 0;
    nvs_handle_t file_handle = 0;

#ifdef LOG
    ESP_LOGI(TAG, "Abrindo %s", fileName.c_str());
#endif
    err = nvs_open_from_partition(FlashConsts::PARTITION_NAME, fileName.c_str(), NVS_READONLY,
                                  &file_handle);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        return ErrorCode(ErrorCodes::FileNotFound);
    } else if (err != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Erro (%s) abrindo NVS handle", esp_err_to_name(err));
        return ErrorCode(ErrorCodes::Error);
    }

    char outputStr[256];
    if (std::is_same<T, std::string>()) {
        err = nvs_get_str(file_handle, key.c_str(), outputStr, &outputSize);
        *reinterpret_cast<std::string *>(&out) = outputStr;
    } else if (std::is_same<T, std::int32_t>()) {
        err = nvs_get_i32(file_handle, key.c_str(), reinterpret_cast<int32_t *>(&out));
    } else if (std::is_same<T, std::uint32_t>()) {
        err = nvs_get_u32(file_handle, key.c_str(), reinterpret_cast<uint32_t *>(&out));
    } else if (std::is_same<T, std::int64_t>()) {
        err = nvs_get_i64(file_handle, key.c_str(), reinterpret_cast<int64_t *>(&out));
    } else if (std::is_same<T, std::uint64_t>()) {
        err = nvs_get_u64(file_handle, key.c_str(), reinterpret_cast<uint64_t *>(&out));
    } else {
        ESP_LOGE(__FUNCTION__, "Tipo invalido ou nao suportado");
    }

    if (err != ESP_OK) {
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            result = ErrorCode(ErrorCodes::KeyNotFound);
        } else {
            result = ErrorCode(ErrorCodes::Error);
        }
        ESP_LOGE(__FUNCTION__, "Erro (%s) lendo dados", esp_err_to_name(err));

        goto end;
    } else {
        nvs_commit(file_handle);
#ifdef LOG
        ESP_LOGI(__FUNCTION__, "Sucesso lendo chave/valor");
#endif
        result = ErrorCode(ErrorCodes::None);
    }

    end:
    nvs_close(file_handle);
    return result;
}