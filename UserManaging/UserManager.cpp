//
// Created by maikeu on 25/09/2019.
//
#include "projectConfig.h"

#ifdef USER_MANAGEMENT_ENABLED

#include <esp_log.h>
#include "UserManager.h"
#include "ConnectedUser.h"
#include "nameof.hpp"
#include "string"
#include "CommandCode.h"

#define DEBUG_INFO

SafeList<ConnectedUser *> UserManager::_activeUsers{};//NOLINT

void UserManager::CreateManager() {
    //Adiciona os Erros
    ErrorCode::AddErrorItem(ErrorCodes::NotConfirmed);
    ErrorCode::AddErrorItem(ErrorCodes::UserNotFound);
    ErrorCode::AddErrorItem(ErrorCodes::WrongPassword);
    ErrorCode::AddErrorItem(ErrorCodes::AdminNotRegistered);
    ErrorCode::AddErrorItem(ErrorCodes::NoUsersRegistered);

    //Adiciona os comandos

    DeviceCommand Login(2, std::string(NAMEOF(Login)), (uint8_t) CommandCode::LoginCode,
                        [this](const std::vector<std::string> &data, BluetoothConnection *connection) {
                            this->Login(data, connection);
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

    Commander::AddCommand(Login);
    Commander::AddCommand(Logoff);
    Commander::AddCommand(SignUp);
    Commander::AddCommand(GetUsersWaiting);
    Commander::AddCommand(ApproveUser);
}

ErrorCode UserManager::LoadUser(const string &userName, JsonModels::User &user) {//NOLINT
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

auto UserManager::SaveUser(const JsonModels::User &user) -> ErrorCode {//NOLINT
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

void UserManager::SignUp(const string &jsonStr, BluetoothConnection *connection) { //NOLINT
    JsonModels::User user{};
    if (!user.FromString(jsonStr)) {
        ESP_LOGE(__FUNCTION__, "Erro tentando deserializar %s como usuario", jsonStr.c_str());
        return;
    }

#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Deserializado=> %s", user.ToString().c_str());
#endif

    //Verifica admin
    std::string isRegistered;
    auto loadAdminResult = Storage::LoadConfig(AdminRegistered, isRegistered);
    if (loadAdminResult == ErrorCodes::KeyNotFound || loadAdminResult == ErrorCodes::FileNotFound ||
        isRegistered == "false") {
        user.IsAdmin = true;
    }

    auto saveResult = SaveUser(user);
    JsonModels::SignUpResultJson result;
    if (saveResult == ErrorCodes::None && user.IsAdmin) {
        Storage::StoreConfig(AdminRegistered, "true", true);
        ESP_LOGI(__FUNCTION__, "Usuario Administrador cadastrado como %s", user.Name.c_str());
        result.IsAdmin = true;
    }

    connection->SendJsonData<JsonModels::SignUpResultJson>(result);
}

void UserManager::GetUsersWaitingForApproval(BluetoothConnection *connection) {
    std::map<std::string, JsonModels::User> usersWaiting;

    auto result = Storage::GetEntriesWithFilter(StorageConst::UsersFilename, usersWaiting,
                                                Utility::FFL([](std::string userName, JsonModels::User user) {//NOLINT
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

void UserManager::ApproveUser(const string &userName, BluetoothConnection *pConnection) {//NOLINT
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

ConnectedUser *UserManager::CreateUserInstance() {
    ESP_LOGI(__FUNCTION__, "Criando um ConnectedUser");
    return new ConnectedUser();
}

#endif

