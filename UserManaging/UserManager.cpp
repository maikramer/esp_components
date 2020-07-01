//
// Created by maikeu on 25/09/2019.
//

#include <esp_log.h>
#include <GeneralUtils.h>
#include "UserManager.h"
#include "ConnectedUser.h"

//#define DEBUG_INFO

void UserManager::Init() {

}

auto UserManager::GetAdminInfoJson() -> std::string {
    nlohmann::json j;
    std::string admin = SdCard::LoadConfig(AdminUserKey);
    j["IsFirstLogin"] = admin.empty();
    return j.dump();
}

void UserManager::SetAdmin(const std::vector<std::string> &data, BluetoothConnection *connection) {
    if (data.size() < 2) {
        ESP_LOGE(__FUNCTION__, "Dados invalidos");
    }

    ConnectedUser *connectedUser = connection->GetUser();
    if (connectedUser == nullptr) {
        ESP_LOGE(__FUNCTION__, "Usuario invalido");
        return;
    }

    std::string user = data[0];
    std::string pw = data[1];

    auto res = SdCard::StoreConfig(AdminUserKey, user, false);
    if (res == StoreResult::Exist) {
        ESP_LOGE(__FUNCTION__, "Erro, ja existe um administrador cadastrado");
        return;
    }

    auto result = SdCard::StoreConfig(AdminPasswordKey, pw, false);
    auto json = UserManager::GetSignUpResultJson(result);
    connection->SendJsonData(json);
}

auto UserManager::LoadUser(const string &userName) -> JsonData::User {
    using json = nlohmann::json;
    auto jsonStr = SdCard::LoadUserJson(userName);
    JsonData::User user = {};
    if (jsonStr.empty())
        return user;
    json j = json::parse(jsonStr);
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Tentando desserializar %s", j.dump().c_str());
#endif
    using JsonData::from_json;
    try {
        user = j;
    } catch (json::exception &e) {
        ESP_LOGE(__FUNCTION__, "Exception: %s", e.what());
        return user;
    }
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Usuario desserializado");
#endif
    if (user.Name.empty()) {
        ESP_LOGE(__FUNCTION__, "Erro carregando usuario");
    }

    return user;
}

auto UserManager::SaveUser(JsonData::User user) -> StoreResult {
    using json = nlohmann::json;
    using JsonData::to_json;
    json j = user;
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Usuario serializado");
#endif
    if (user.Name.empty()) {
        ESP_LOGE(__FUNCTION__, "Usuario invalido");
        return StoreResult::Error;
    }
    auto result = SdCard::StoreUserJson(user.Name, j.dump(), false);
    switch (result) {
        case StoreResult::Ok:
#ifdef DEBUG_INFO
            ESP_LOGI(__FUNCTION__, "Usuario %s salvo", user.Name.c_str());
#endif
            break;
        case StoreResult::Exist:
            ESP_LOGW(__FUNCTION__, "Usuario %s existe", user.Name.c_str());
            break;
        case StoreResult::Error:
            ESP_LOGE(__FUNCTION__, "Erro ao salvar o usuario %s", user.Name.c_str());
            break;
    }

    return result;
}

void UserManager::SignUp(const string &jsonStr, BluetoothConnection *connection) {
    using json = nlohmann::json;
    json j = json::parse(jsonStr);
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Tentando desserializar %s", j.dump().c_str());
#endif
    using JsonData::from_json;
    JsonData::User user;
    try {
        user = j;
    } catch (json::exception &e) {
        ESP_LOGE(__FUNCTION__, "Exception: %s", e.what());
        return;
    } catch (...) {
        ESP_LOGE(__FUNCTION__, "Erro");
        return;
    }

    auto *connectedUser = connection->GetUser();
    connectedUser->User = user.Name;
    auto result = SaveUser(user);
    auto resultJson = UserManager::GetSignUpResultJson(result);
    connection->SendJsonData(resultJson);
}

auto UserManager::GetSignUpResultJson(StoreResult result) -> string {

    string resultStr;
    switch (result) {
        case StoreResult::Ok:
            resultStr = "Ok";
            break;
        case StoreResult::Exist:
            resultStr = "Exist";
            break;
        case StoreResult::Error:
            resultStr = "Error";
            break;
    }

    nlohmann::json j;
    j["Result"] = result;
    return j.dump();
}

void UserManager::GetUsersWaitingForApproval(BluetoothConnection *connection) {
    std::list<JsonData::User> usersWaiting;

    usersWaiting = SdCard::GetUserListWithFilter(
            [](const JsonData::User &user) { return !user.IsConfirmed; });

    for (auto waiting:usersWaiting) {
        nlohmann::json j;
        j = waiting;

        auto json_str = j.dump();
//                ESP_LOGI(__FUNCTION__, "Sending %s", json_str.c_str());
        connection->SendJsonData(json_str);
    }

    nlohmann::json j;
    j["End"] = true;
    auto json_str = j.dump();
    connection->SendJsonData(json_str);
}

void UserManager::ApproveUser(const std::string &userName) {
    using json = nlohmann::json;
    auto userLoaded = LoadUser(userName);
    userLoaded.IsConfirmed = true;
    json j;
    try {
        JsonData::to_json(j, userLoaded);
    } catch (json::exception &e) {
        ESP_LOGE(__FUNCTION__, "Exception: %s", e.what());
        return;
    } catch (...) {
        ESP_LOGE(__FUNCTION__, "Erro");
        return;
    }

#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Usuario Serializado");
#endif
    auto json_str = j.dump();
    SdCard::StoreUserJson(userName, json_str, true);

}

void UserManager::GetUsersAccumulatedUsageMap(std::map<std::string, uint32_t> &map, uint32_t from, uint32_t to) {
    auto list = SdCard::GetUserListWithFilter(
            [](const JsonData::User &user) { return true; });

    map.clear();
    for (const auto &user:list) {
        auto usage = SdCard::GetAccumulatedFromUser(user.Name, from, to);
        //So adiciona se nao for zero
        if (usage > 0) {
            auto pair = std::pair<std::string, uint32_t>(user.Name, usage);
            map.insert(pair);
        }
    }

    ESP_LOGI(__FUNCTION__, "Size: %u", map.size());
}

void UserManager::GetUsersAccumulated(const std::vector<std::string> &data, BluetoothConnection *connection) {
    auto dateFrom = std::stoul(data[0]);
    auto dateTo = std::stoul(data[1]);
    std::map<std::string, uint32_t> usageMap;
    GetUsersAccumulatedUsageMap(usageMap, dateFrom, dateTo);
    for (auto usage:usageMap) {
        nlohmann::json j;
        j["UserName"] = usage.first;
        j["Usage"] = usage.second;

        auto json_str = j.dump();
//                ESP_LOGI(__FUNCTION__, "Sending %s", json_str.c_str());
        connection->SendJsonData(json_str);
    }

    nlohmann::json j;
    j["End"] = true;
    auto json_str = j.dump();
    connection->SendJsonData(json_str);

}
