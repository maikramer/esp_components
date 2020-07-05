//
// Created by maikeu on 31/12/2019.
//

#ifdef USE_SDCARD

#ifdef USER_MANAGEMENT_ENABLED
#include "SdCard.h"
#include <JsonData.h>
#include <GeneralUtils.h>
#include <fstream>
#include <sstream>

auto SdCard::GetEntriesWithFilter(const std::string &user, std::map<uint32_t, uint32_t> &map,
                                  boolFunctionPointer filter) -> bool {
    std::stringstream str{};
    str << "/sdcard/" << user << ".txt";
    auto path = str.str();
    std::ifstream ifs(path, std::ifstream::in);
    auto res = true;
    std::string line{};
    if (!ifs.is_open()) {
        ESP_LOGE(__FUNCTION__, "Erro Abrindo arquivo");
        res = false;
        goto end;
    }

    map.clear();

    while (std::getline(ifs, line)) {
        if (line.empty()) {
            ESP_LOGW(__FUNCTION__, "Linha vazia");
            continue;
        }

        auto keyValue = GeneralUtils::split(line, '=');
        if (keyValue.empty()) {
            ESP_LOGW(__FUNCTION__, "Erro na linha");
            continue;
        }

        auto key = strtol(keyValue[0].c_str(), nullptr, 10);
        auto value = strtol(keyValue[1].c_str(), nullptr, 10);

        if (filter()) {
            map[key] = value;
        }

    }

    ifs.close();

    end:
    return res;
}

auto SdCard::GetEntriesFromUser(const std::string &user, std::map<uint32_t, uint32_t> &map) -> bool {
    return GetEntriesWithFilter(user, map, [] { return true; });
}

auto SdCard::GetUserListWithFilter(boolFunctionPointerUser filter) -> std::list<JsonData::User> {
    const char *TAG = __FUNCTION__;
    const char *path = "/sdcard/users.txt";
    auto list = std::list<JsonData::User>();
    std::ifstream input(path, std::ifstream::in);
    std::string line{};
    if (!input) {
        ESP_LOGE(TAG, "Arquivo users.txt nao encontrado ou nao criado ainda");
        goto end;
    }

    while (input >> line) {
        if (line.empty()) {
            ESP_LOGW(TAG, "Linha vazia");
            continue;
        }

        auto eq = line.find('=');
        if (eq == std::string::npos) {
            ESP_LOGW(TAG, "Igual nao encotrado na linha");
            continue;
        }

        auto value = line.substr(eq + 1);

        using json = nlohmann::json;
        json j = json::parse(value);
        using JsonData::from_json;
        JsonData::User user;
        try {
            user = j;
        } catch (json::exception &e) {
            ESP_LOGE(TAG, "Exception: %s", e.what());
            break;
        } catch (...) {
            break;
        }

        ESP_LOGI(TAG, "Usuario desserializado");
        if (user.Name.empty()) {
            ESP_LOGE(TAG, "Erro carregando usuario");
            break;
        }

        if (filter(user)) {
            list.push_back(user);
        }
    }

    input.close();

    end:
    return list;
}

auto
SdCard::StoreUserJson(const std::string &username, const std::string &userJson, bool overwrite) -> StoreResult {
    const char *TAG = __FUNCTION__;

    ESP_LOGI(TAG, "Opening user file");

    auto res = StoreKeyValue(username, userJson, "users", overwrite);
    switch (res) {

        case StoreResult::Ok:
            ESP_LOGI(TAG, "Usuario Salvo");
            break;
        case StoreResult::Exist:
            ESP_LOGW(TAG, "Usuario ja existe");
            break;
        case StoreResult::Error:
            ESP_LOGE(TAG, "Erro salvando configuracao");
            break;
    }

    return res;
}

auto SdCard::LoadUserJson(const std::string &username) -> std::string {
    const char *TAG = __FUNCTION__;

    ESP_LOGI(TAG, "Opening User file");
    auto ret = ReadKeyFromFile(username, "users.txt");

    if (ret.empty()) {
        ESP_LOGE(TAG, "Erro lendo usuario %s do arquivo", username.c_str());
    }

    return ret;
}

auto SdCard::GetAccumulatedFromUser(const std::string &apTorre, const uint32_t from, const uint32_t to) -> uint32_t {
    auto map = std::map<uint32_t, uint32_t>();
    if (!GetEntriesFromUser(apTorre, map)) {
        return 0;
    }

    uint32_t accum = 0;
    for (auto m : map) {
        if ((from == 0 || m.first > from) && (to == 0 || m.first < to)) {
            accum += m.second;
        }
    }

    ESP_LOGI(__FUNCTION__, "%u Registros encontrados!\nTotal acumulado nos Registros: %u", map.size(), accum);
    return accum;
}
#endif

#endif