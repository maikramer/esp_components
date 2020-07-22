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
#include "CommandCode.h"

#define DEBUG_INFO

void UserManager::Init() {
    //Adiciona os Erros
    ErrorCode::AddErrorItem(ErrorCodes::NotConfirmed);
    ErrorCode::AddErrorItem(ErrorCodes::UserNotFound);
    ErrorCode::AddErrorItem(ErrorCodes::WrongPassword);
    ErrorCode::AddErrorItem(ErrorCodes::AdminNotRegistered);
    ErrorCode::AddErrorItem(ErrorCodes::NoUsersRegistered);

    //Ediciona os comandos
    const DeviceCommand SetAdminInfo(2, std::string(
    NAMEOF(
            SetAdminInfo)), (uint8_t) CommandCode::SetAdminInfoCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::SetAdmin(data, connection);
            });

    const DeviceCommand Login(2, std::string(NAMEOF(Login)), (uint8_t) CommandCode::LoginCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::Login(data, connection);
            });

    const DeviceCommand Logoff(0, std::string(NAMEOF(Logoff)), (uint8_t) CommandCode::LogoffCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::Logoff(connection);
            });

    const DeviceCommand SignUp(1, std::string(NAMEOF(SignUp)), (uint8_t) CommandCode::SignUpCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::SignUp(data[0], connection);
            });

    const DeviceCommand GetUsersWaiting(0, std::string(
    NAMEOF(
            GetUsersWaiting)), (uint8_t) CommandCode::GetUsersWaitingCode,
            [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                UserManager::GetUsersWaitingForApproval(connection);
            });

    const DeviceCommand ApproveUser(1, std::string(
    NAMEOF(
            ApproveUser)), (uint8_t) CommandCode::ApproveUserCode,
            [](const std::vector<std::string> &data,
               BluetoothConnection *connection) {
                UserManager::ApproveUser(data[0], connection);
            });

    Commander::AddCommand(SetAdminInfo);
    Commander::AddCommand(Login);
    Commander::AddCommand(Logoff);
    Commander::AddCommand(SignUp);
    Commander::AddCommand(GetUsersWaiting);
    Commander::AddCommand(ApproveUser);
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

ErrorCode UserManager::LoadUser(const string &userName, JsonModels::User &user) {
    auto result = Storage::LoadUser(userName, user);
    if (result == ErrorCodes::FileNotFound or result == ErrorCodes::KeyNotFound) {
        return ErrorCodes::UserNotFound;
    }

    if (result != ErrorCodes::None) {
        ESP_LOGE(__FUNCTION__, "Erro carregando usuario, Erro: %s", result.ToString());
    }

    if (!user.IsValid()) {
        ESP_LOGE(__FUNCTION__, "Usuário Inválido => %s", user.ToString().c_str());
    }

    return result;
}

auto UserManager::SaveUser(const JsonModels::User &user) -> ErrorCode {
    if (user.Name.empty()) {
        ESP_LOGE(__FUNCTION__, "Usuario invalido");
        return ErrorCodes::Error;
    }

    auto result = Storage::StoreUser(user, false);
    if (result == ErrorCodes::None) {
#ifdef DEBUG_INFO
        ESP_LOGI(__FUNCTION__, "Usuario %s salvo", user.Name.c_str());
#endif
    } else if (result == ErrorCodes::Exist) {
        ESP_LOGW(__FUNCTION__, "Usuario %s existe", user.Name.c_str());
    } else {
        ESP_LOGE(__FUNCTION__, "Erro ao salvar o usuario %s", user.Name.c_str());
    }

    return result;
}

void UserManager::SignUp(const string &jsonStr, BluetoothConnection *connection) {


    auto *connectedUser = connection->GetUser();
    JsonModels::User user{};
    if (!user.FromString(jsonStr)) {
        ESP_LOGE(__FUNCTION__, "Erro tentando desserializar %s como usuario", jsonStr.c_str());
    }

#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Disserializado=> %s", user.ToString().c_str());
#endif

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
        if (result == ErrorCodes::FileNotFound) {
            connection->SendError<JsonModels::UserListJsonData>(ErrorCodes::NoUsersRegistered);
        }

        return;
    }

    connection->SendList<JsonModels::UserListJsonData>(usersWaiting);
}

void UserManager::ApproveUser(const string &userName, BluetoothConnection *pConnection) {
    JsonModels::User user{};
    auto result = LoadUser(userName, user);
    if (result != ErrorCodes::None) {
        ESP_LOGE(__FUNCTION__, "Erro Carregando o usuario %s", userName.c_str());
        pConnection->SendError<JsonModels::BaseJsonDataError>(result);
    }

    user.IsConfirmed = true;
    auto res = Storage::StoreUser(user, true);
    if (res != ErrorCodes::None) {
        ESP_LOGE(__FUNCTION__, "Erro gravando o usuario %s", user.Name.c_str());
        pConnection->SendError<JsonModels::BaseJsonDataError>(res);
    }
}

#endif

