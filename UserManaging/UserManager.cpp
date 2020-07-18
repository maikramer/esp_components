//
// Created by maikeu on 25/09/2019.
//
#include "projectConfig.h"

#ifdef USER_MANAGEMENT_ENABLED

#include <esp_log.h>
#include <GeneralUtils.h>
#include "UserManager.h"
#include "ConnectedUser.h"
#include "nameof.hpp"
#include "string"

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
    const DeviceCommand SetAdminInfo(2, std::string(
    NAMEOF(
            SetAdminInfo)), (uint8_t) UserManagerCommandCode::SetAdminInfoCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::SetAdmin(data, connection);
            });

    const DeviceCommand Login(2, std::string(NAMEOF(Login)), (uint8_t) UserManagerCommandCode::LoginCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::Login(data, connection);
            });

    const DeviceCommand Logoff(0, std::string(NAMEOF(Logoff)), (uint8_t) UserManagerCommandCode::LogoffCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::Logoff(connection);
            });

    const DeviceCommand SignUp(1, std::string(NAMEOF(SignUp)), (uint8_t) UserManagerCommandCode::SignUpCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::SignUp(data[0], connection);
            });

    const DeviceCommand GetUsersWaiting(0, std::string(
    NAMEOF(
            GetUsersWaiting)), (uint8_t) UserManagerCommandCode::GetUsersWaitingCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::GetUsersWaitingForApproval(connection);
            });

    const DeviceCommand ApproveUser(1, std::string(
    NAMEOF(
            ApproveUser)), (uint8_t) UserManagerCommandCode::ApproveUserCode,
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
    std::string admin = Storage::LoadConfig(AdminUserKey);
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

    auto res = Storage::StoreConfig(AdminUserKey, user, false);
    if (res == ErrorCodes::Exist) {
        ESP_LOGE(__FUNCTION__, "Erro, ja existe um administrador cadastrado");
        return;
    }

    auto result = Storage::StoreConfig(AdminPasswordKey, pw, false);
    connection->SendError<JsonModels::BaseJsonDataError>(result);
}

auto UserManager::LoadUser(const string &userName) -> JsonModels::User {
    JsonModels::User user = {};
    auto result = Storage::LoadUser(userName, user);
    if (result != ErrorCodes::None) {
        ESP_LOGE(__FUNCTION__, "Erro carregando usuario, codigo: %d", (uint8_t) result);
    }
    if (user.Name.empty()) {
        ESP_LOGE(__FUNCTION__, "Erro carregando usuario, nome vazio");
    }

    return user;
}

auto UserManager::SaveUser(const JsonModels::User &user) -> ErrorCode {
    if (user.Name.empty()) {
        ESP_LOGE(__FUNCTION__, "Usuario invalido");
        return ErrorCodes::Error;
    }

    auto result = Storage::StoreUser(user, false);
    switch ((uint8_t) result) {
        case ErrorCodes::None:
#ifdef DEBUG_INFO
            ESP_LOGI(__FUNCTION__, "Usuario %s salvo", user.Name.c_str());
#endif
            break;
        case ErrorCodes::Exist:
            ESP_LOGW(__FUNCTION__, "Usuario %s existe", user.Name.c_str());
            break;
        case ErrorCodes::Error:
            ESP_LOGE(__FUNCTION__, "Erro ao salvar o usuario %s", user.Name.c_str());
            break;
        case ErrorCodes::FileNotFound:
        case ErrorCodes::KeyNotFound:
        case ErrorCodes::FindError:
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

    auto *connectedUser = connection->GetUser();
    JsonModels::User user{};
    if (!user.FromString(jsonStr)) {
        ESP_LOGE(__FUNCTION__, "Erro tentando desserializar %s como usuario", jsonStr.c_str());
    }

    connectedUser->User = user.Name;
    auto result = SaveUser(user);
    connection->SendError<JsonModels::BaseJsonDataError>(result);
}

void UserManager::GetUsersWaitingForApproval(BluetoothConnection *connection) {
    std::map<std::string, JsonModels::User> usersWaiting;

    auto result = Storage::GetEntriesWithFilter(StorageConst::UsersFilename, usersWaiting,
                                                Utility::FFL([](std::string userName, JsonModels::User user) {
                                                    return !user.IsConfirmed;
                                                }));

    if (result != ErrorCodes::None) {
        connection->SendError<JsonModels::UserListJsonData>(ErrorCodes::StorageError);
        return;
    }

    connection->SendList<JsonModels::UserListJsonData>(usersWaiting);
}

//todo: Gerenciar melhor os erros
void UserManager::ApproveUser(const std::string &userName) {
    auto userLoaded = LoadUser(userName);
    userLoaded.IsConfirmed = true;
    if (Storage::StoreUser(userLoaded, true) != ErrorCodes::None) {
        ESP_LOGE(__FUNCTION__, "Erro aprovando o usuario %s", userLoaded.Name.c_str());
    }

}

#endif

