//
// Created by maikeu on 25/09/2019.
//

#include <esp_log.h>
#include <GeneralUtils.h>
#include "UserManager.h"
#include "ConnectedUser.h"

//#define DEBUG_INFO
//todo:Adicionar uma condicao que os codigos devem ser maiores que 10, mudar os codigos de main
enum class UserManagerCommandCode : uint8_t {
    SetAdminInfoCode = 5,
    LoginCode = 6,
    LogoffCode = 7,
    SignUpCode = 8,
    GetUsersWaitingCode = 9,
    ApproveUserCode = 10
};

void UserManager::Init() {
    const DeviceCommand SetAdminInfo(2, "SetAdminInfo", (uint8_t) UserManagerCommandCode::SetAdminInfoCode,
                                     [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                         UserManager::SetAdmin(data, connection);
                                     });

    const DeviceCommand Login(2, "Login", (uint8_t) UserManagerCommandCode::LoginCode,
                              [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                  UserManager::Login(data, connection);
                              });

    const DeviceCommand Logoff(0, "Logoff", (uint8_t) UserManagerCommandCode::LogoffCode,
                               [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                   UserManager::Logoff(connection);
                               });

    const DeviceCommand SignUp(1, "SignUp", (uint8_t) UserManagerCommandCode::SignUpCode,
                               [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                   UserManager::SignUp(data[0], connection);
                               });

    const DeviceCommand GetUsersWaiting(0, "GetUsersWaiting", (uint8_t) UserManagerCommandCode::GetUsersWaitingCode,
                                        [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                            UserManager::GetUsersWaitingForApproval(connection);
                                        });

    const DeviceCommand ApproveUser(1, "ApproveUser", (uint8_t) UserManagerCommandCode::ApproveUserCode,
                                    [](const std::vector<std::string> &data,
                                       BluetoothConnection *connection) {
                                        UserManager::ApproveUser(data[0]);
                                    });

    Commander::AddCommand(SetAdminInfo);
    Commander::AddCommand(Login);
    Commander::AddCommand(Logoff);
    Commander::AddCommand(SignUp);
    Commander::AddCommand(GetUsersWaiting);
    Commander::AddCommand(ApproveUser);
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


