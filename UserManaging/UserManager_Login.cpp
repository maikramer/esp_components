//
// Created by maikeu on 23/02/2020.
//
#include "projectConfig.h"

#ifdef USER_MANAGEMENT_ENABLED

#include <esp_log.h>
#include <SdCard.h>
#include "UserManager.h"
#include "ConnectedUser.h"

#define DEBUG_INFO

void UserManager::Login(const std::vector<std::string> &data, BluetoothConnection *connection) {
    auto *connectedUser = connection->GetUser();

    //Verifica admin
    std::string adminUser;
    auto result = Storage::LoadConfig(AdminUserKey, adminUser);
    if (result == ErrorCodes::KeyNotFound || result == ErrorCodes::FileNotFound) {
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

    ESP_LOGI(__FUNCTION__, "User: %s | Admin: %s", user.c_str(), adminUser.c_str());
    //Verifica usuario
    if (user == adminUser) {
        std::string adminPw;
        auto pwRes = Storage::LoadConfig(AdminPasswordKey, adminPw);
        if (pwRes != ErrorCodes::None) {
            connection->SendError<JsonModels::LoginTryResultJson>(pwRes);
        }

        if (pw == adminPw) {
            connectedUser->IsAdmin = true;
            connectedUser->IsLogged = true;
            connectedUser->User = user;
            result = ErrorCodes::None;
        } else {
            result = ErrorCodes::WrongPassword;
        }
    } else {
        result = CheckPassword(user, pw);
        if (result == ErrorCodes::None) {
            connectedUser->IsLogged = true;
            connectedUser->User = user;
            connectedUser->OnLogin();
        }
    }

    ESP_LOGI(__FUNCTION__, "Enviando o resultado: %s", result.GetName());
    SendLoginTryResult(result, connection);
}

void UserManager::SendLoginTryResult(ErrorCode errorCode, BluetoothConnection *connection) {
    auto *user = connection->GetUser();
    JsonModels::LoginTryResultJson result{};
    result.ErrorMessage = errorCode;
    result.IsAdmin = user->IsAdmin;

    auto json_str = result.ToJson();
    ESP_LOGI(__FUNCTION__, "Enviando => %s", json_str.c_str());
    connection->SendJsonData(json_str);
}


ErrorCode UserManager::CheckPassword(string &userName, string &pass) {
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

    if (!user.IsConfirmed) {
        return ErrorCodes::NotConfirmed;
    }

    if (user.Password != pass) {
        return ErrorCodes::WrongPassword;
    } else {
        return ErrorCodes::None;
    }
}

void UserManager::Logoff(BluetoothConnection *connection) {
    auto *connectedUser = connection->GetUser();
    connectedUser->Clear();

    JsonModels::BaseJsonDataError error;
    error.ErrorMessage = !connectedUser->IsLogged ? ErrorCodes::None : ErrorCodes::Error;
    auto json = error.ToJson();
    connection->SendJsonData(json);
}

#endif