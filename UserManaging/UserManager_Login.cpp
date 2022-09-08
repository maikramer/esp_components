//
// Created by maikeu on 23/02/2020.
//
#include "projectConfig.h"

#ifdef USER_MANAGEMENT_ENABLED

#include <esp_log.h>
#include "UserManager.h"
#include "ConnectedUser.h"

#define DEBUG_INFO

void
UserManager::Login(const std::vector<std::string> &data, BluetoothConnection *connection) {//NOLINT

    //Verifica admin
    std::string adminRegistered;
    auto result = Storage::LoadConfig(AdminRegistered, adminRegistered);
    if (result == ErrorCodes::KeyNotFound || result == ErrorCodes::FileNotFound ||
        adminRegistered == "false") {
        connection->SendError<JsonModels::LoginTryResultJson>(ErrorCodes::AdminNotRegistered);
        return;
    } else if (result != ErrorCodes::None) {
        connection->SendError<JsonModels::LoginTryResultJson>(result);
        return;
    }

    std::string user = data[0];
    std::string pw;
    if (data.size() > 1) {
        pw = data[1];
    } else {
        ESP_LOGE(__FUNCTION__, "Usuário não contem senha");
        connection->SendError<JsonModels::LoginTryResultJson>(ErrorCodes::Error);
    }

    ConnectedUser *connectedUser = nullptr;
    for (auto *active: _activeUsers) {
        if (active != nullptr && !active->User.empty() && active->User == user) {
            connectedUser = active;
            ESP_LOGI(__FUNCTION__, "Encontrado usuario travado, retomando");
            connectedUser->IsContinuingToUse = true;
            break;
        }
    }
    if (connectedUser == nullptr) {
        connectedUser = CreateUserInstance();
    }

    //Verifica usuario

    bool isAdmin = false;
    result = CheckPassword(user, pw, isAdmin);
    if (result == ErrorCodes::None) {
        result = connectedUser->Login(isAdmin, user);
    }

    connection->SetUser(connectedUser);

    ESP_LOGI(__FUNCTION__, "Enviando o resultado: %s", result.GetName());
    SendLoginTryResult(result, connection);
}

void UserManager::SendLoginTryResult(ErrorCode errorCode, BluetoothConnection *connection) {
    auto *user = connection->GetUser(false, false);
    JsonModels::LoginTryResultJson result{};
    result.ErrorMessage = errorCode;
    result.IsAdmin = user->IsAdmin;

    auto json_str = result.ToJson();
    ESP_LOGI(__FUNCTION__, "Enviando => %s", json_str.c_str());
    connection->SendJsonData(json_str);
}


ErrorCode UserManager::CheckPassword(string &userName, string &pass, bool &isAdmin) {
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Tentativa User:%s , Senha:%s", userName.c_str(), pass.c_str());
#endif
    JsonModels::User user{};
    auto result = LoadUser(userName, user);
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Carregado => %s", user.ToString().c_str());
#endif

    if (result != ErrorCodes::None) {
        return result;
    }

    if (user.Password != pass) {
        return ErrorCodes::WrongPassword;
    }

    if (!user.IsAdmin && !user.IsConfirmed) {
        return ErrorCodes::NotConfirmed;
    }

    isAdmin = user.IsAdmin;
    return ErrorCodes::None;
}

void UserManager::Disconnect(ConnectedUser *user) {
    if (user == nullptr) {
        ESP_LOGE(__FUNCTION__, "Usuario nulo");
        return;
    }

    auto res = user->Disconnect();
    if (res) {
        DeleteUser(user);
    }
}

void UserManager::Logoff(BluetoothConnection *connection) {
    auto *connectedUser = connection->GetUser(false, false);
    connection->Logoff();
    auto isLocked = !connectedUser->Logoff();
    if (!isLocked) {
        DeleteUser(connectedUser);
        connectedUser = nullptr;
    }
    JsonModels::BaseJsonDataError error;
    bool isLockedAndLoggedOff = connectedUser != nullptr && !connectedUser->IsLogged;
    ESP_LOGI(__FUNCTION__, "isLocked: %u, isLockedAndLoggedOff: %u", isLocked,
             isLockedAndLoggedOff);
    error.ErrorMessage = isLockedAndLoggedOff || !isLocked ? ErrorCodes::None : ErrorCodes::Error;
    auto json = error.ToJson();
    connection->SendJsonData(json);
}

#endif