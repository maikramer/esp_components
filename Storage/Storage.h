//
// Created by maikeu on 23/09/2019.
//

#ifndef ROCKET_TESTER_STORAGE_H
#define ROCKET_TESTER_STORAGE_H


#include <cstdint>
#include <map>
#include <list>
#include <JsonData.h>
#include <GeneralUtils.h>
#include "functional"
#include "Utility.h"
#include "sstream"
#include "fstream"

#define LOG_STORAGE

enum class StoreResult : uint8_t {
    Ok = 0,
    Exist = 1,
    FileNotFound = 2,
    KeyNotFound = 3,
    FindError = 4,
    Error = 5
};

enum class MountError {
    Ok,
    PartitionNotFound,
    AlreadyMounted,
    NoFreeMemory,
    Error
};

namespace StorageConst {
    static const char *BasePath = "/storage";
}

struct StorageStatus {
    uint64_t FreeSpace;
    uint64_t TotalSpace;
};

class Storage {

public:
    static auto EraseData() -> bool;

    template<typename Tkey, typename Tvalue>
    static auto
    StoreKeyValue(Tkey key, Tvalue value, const std::string &fileName,
                  bool overwrite) -> StoreResult;

    template<typename Tkey, typename Tvalue>
    static auto ReadKeyFromFile(Tkey key, Tvalue &out, const std::string &fileName) -> StoreResult;

    template<typename Tkey, typename Tvalue>
    static auto GetEntriesWithFilter(const std::string &fileName, std::map<Tkey, Tvalue> &map,
                                     std::function<bool(std::string key, std::string value)> filter) -> StoreResult;

    template<typename Tkey, typename Tvalue>
    static auto
    FastStoreKeyValue(Tkey key, Tvalue value, const std::string &fileName) -> StoreResult;

    static StoreResult GetStorageStatus(StorageStatus &status);

protected:
    static uint32_t _sectorSize;

    friend class Flash;

};


template<typename Tkey, typename Tvalue>
auto Storage::GetEntriesWithFilter(const std::string &fileName, std::map<Tkey, Tvalue> &map,
                                   std::function<bool(std::string, std::string)> filter) -> StoreResult {
    std::stringstream str{};
    str << StorageConst::BasePath << "/" << fileName << ".txt";
    auto path = str.str();
    std::ifstream ifs(path, std::ifstream::in);
    StoreResult res = StoreResult::Ok;
    std::string line{};
    Tkey outKey;
    Tvalue outValue;
    if (!ifs.is_open()) {
        ESP_LOGE(__FUNCTION__, "Erro Abrindo arquivo");
        res = StoreResult::FileNotFound;
        goto end;
    }

    map.clear();

    while (std::getline(ifs, line)) {
        if (line.empty()) {
            ESP_LOGW(__FUNCTION__, "Linha vazia");
            continue;
        }
#ifdef LOG_STORAGE
        ESP_LOGI(__FUNCTION__, "Linha >> %s", line.c_str());
#endif

        auto keyValue = GeneralUtils::split(line, '=');
        if (keyValue.empty()) {
            ESP_LOGW(__FUNCTION__, "Erro na linha");
            continue;
        }

        try {
            outKey = Utility::GetConvertedFromString<Tkey>(keyValue[0]);
            outValue = Utility::GetConvertedFromString<Tvalue>(keyValue[1]);
        } catch (std::exception &e) {
            continue;
        }


        if (std::invoke(filter, keyValue[0], keyValue[1])) {
            map[outKey] = outValue;
        }
    }

    ifs.close();

    if (map.size() > 0) {
#ifdef LOG_STORAGE
        ESP_LOGI(__FUNCTION__, "%d valores inseridos ao mapa", map.size());
#endif
        res = StoreResult::Ok;
    }

    end:
    return res;
}


template<typename Tkey, typename Tvalue>
auto Storage::StoreKeyValue(Tkey key, Tvalue value, const std::string &fileName,
                            bool overwrite) -> StoreResult {
    StoreResult result = StoreResult::Ok;
    std::stringstream str{};
    std::stringstream keyStrStr;
    keyStrStr << key;
    str << StorageConst::BasePath << "/" << fileName << ".txt";
    auto path = str.str();
#ifdef LOG_STORAGE
    ESP_LOGI(__FUNCTION__, "Opening %s", path.c_str());
#endif
    std::ifstream input(path, std::ifstream::in); //File to read from
    if (!input) {
#ifdef LOG_STORAGE
        ESP_LOGI(__FUNCTION__, "Arquivo nao encontrado, criando....");
#endif
        std::ofstream ofs(path, std::ofstream::app);
        ofs << key << '=' << value << std::endl;
        ofs.close();
    } else {
        std::stringstream tempStr{};
        tempStr << StorageConst::BasePath << "/" << "temp.txt";
        auto tempPath = tempStr.str();
        std::ofstream ofs(tempStr.str(), std::ofstream::app);
        if (!ofs) {
            ESP_LOGE(__FUNCTION__, "Erro Abrindo arquivos");
            result = StoreResult::Error;
            goto error;
        }

        std::string line{};
        bool found = false;
        while (std::getline(input, line)) {
            if (line.empty()) {
                ESP_LOGW(__FUNCTION__, "Linha vazia");
                continue;
            }
#ifdef LOG_STORAGE
            ESP_LOGI(__FUNCTION__, "Linha >> %s", line.c_str());
#endif
            if (!found) {
                auto findKeyRes = GeneralUtils::split(line, '=');
                if (!findKeyRes.empty() && findKeyRes[0] == keyStrStr.str()) {
                    found = true;
                }
            }
            if (found) {
                if (overwrite) {
#ifdef LOG_STORAGE
                    ESP_LOGI(__FUNCTION__, "Chave encontrada, atualizando");
#endif
                    ofs << key << '=' << value << std::endl;
                } else {
                    ofs << line << std::endl;
#ifdef LOG_STORAGE
                    ESP_LOGI(__FUNCTION__, "Chave encontrada, mas, nao deve ser atualizada");
#endif
                    result = StoreResult::Exist;
                }
            } else {
                ofs << line << std::endl;
            }
        }

        if (!found) {
            ofs << key << '=' << value << std::endl;
#ifdef LOG_STORAGE
            std::stringstream strOut{};
            strOut << key << '=' << value << std::endl;
            ESP_LOGI(__FUNCTION__, "Gravado >> %s", strOut.str().c_str());
#endif
        }

        ofs.close();
        input.close();

        if (found && !overwrite) {
            remove(tempPath.c_str());
        } else {
            if (remove(path.c_str()) != 0) {
                ESP_LOGE(__FUNCTION__, "Erro Apagando arquivo Antigo");
                result = StoreResult::Error;
                goto error;
            } else {
#ifdef LOG_STORAGE
                ESP_LOGI(__FUNCTION__, "Arquivo Antigo apagado");
#endif
            }

            if (rename(tempPath.c_str(), path.c_str()) != 0) {
                ESP_LOGE(__FUNCTION__, "Erro Renomeando Temp");
                result = StoreResult::Error;
            } else {
#ifdef LOG_STORAGE
                ESP_LOGI(__FUNCTION__, "Temp renomeado para %s", tempPath.c_str());
#endif
            }
        }
    }

#ifdef LOG_STORAGE
    ESP_LOGI(__FUNCTION__, "Arquivo %s salvo", path.c_str());
#endif
    error:
    return result;
}

template<typename Tkey, typename Tvalue>
auto Storage::ReadKeyFromFile(Tkey key, Tvalue &out, const std::string &fileName) -> StoreResult {
    std::stringstream pathStr{};
    StoreResult res;
    pathStr << StorageConst::BasePath << "/" << fileName << ".txt";
    auto path = pathStr.str();
#ifdef LOG_STORAGE
    ESP_LOGI(__FUNCTION__, "Opening %s", path.c_str());
#endif
    std::ifstream input(path, std::ifstream::in);
    std::string line{};
    std::stringstream keyStrStr;
    keyStrStr << key;
    if (!input) {
        ESP_LOGE(__FUNCTION__, "Arquivo %s nao encontrado ou nao criado ainda", path.c_str());
        res = StoreResult::FileNotFound;
        goto end;
    }

    while (std::getline(input, line)) {
        if (line.empty()) {
            ESP_LOGW(__FUNCTION__, "Linha vazia");
            continue;
        }
#ifdef LOG_STORAGE
        ESP_LOGI(__FUNCTION__, "Linha >> %s", line.c_str());
#endif
        auto findKeyRes = GeneralUtils::split(line, '=');
        if (findKeyRes.size() < 2) {
            ESP_LOGE(__FUNCTION__, "Linha com formato invalido >> %s", line.c_str());
            continue;
        }
        if (findKeyRes[0] == keyStrStr.str()) {
            out = Utility::GetConvertedFromString<Tvalue>(findKeyRes[1]);
            res = StoreResult::Ok;
#ifdef LOG_STORAGE
            ESP_LOGI(__FUNCTION__, "Lido %s da chave %s", findKeyRes[0].c_str(), findKeyRes[0].c_str());
#endif
            break;
        }
    }

    input.close();

    end:
    return res;
}

template<typename Tkey, typename Tvalue>
auto Storage::FastStoreKeyValue(Tkey key, Tvalue value,
                                const std::string &fileName) -> StoreResult {
#ifdef LOG_STORAGE
    const char *TAG = __FUNCTION__;
    ESP_LOGI(TAG, "Opening file");
#endif
    std::stringstream str{};
    str << StorageConst::BasePath << "/" << fileName << ".txt";
    auto path = str.str();
    std::ofstream ofs(path, std::ofstream::app);
    if (!ofs) {
        ESP_LOGE(TAG, "Erro abrindo ou criando arquivo");
        return StoreResult::Error;
    }

    ofs << key << '=' << value << std::endl;
#ifdef LOG_STORAGE
    std::stringstream strOut{};
    strOut << key << '=' << value << std::endl;
    ESP_LOGI(TAG, "Gravado >> %s", strOut.str().c_str());
#endif
    ofs.close();

#ifdef LOG_STORAGE
    ESP_LOGI(TAG, "File written");
#endif
    return StoreResult::Ok;
}

#endif //ROCKET_TESTER_STORAGE_H
