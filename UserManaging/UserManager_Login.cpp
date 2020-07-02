//
// Created by maikeu on 23/02/2020.
//

#include <esp_log.h>
#include "UserManager.h"

void UserManager::Login(const std::vector<std::string> &data, BluetoothConnection *connection) {
    auto *connectedUser = connection->GetUser();

    //Verifica admin
    bool isFirstAdminLogin = false;
    auto adminUser = SdCard::LoadConfig(AdminUserKey);
    if (adminUser.empty()) {
        adminUser = "Admin";
        isFirstAdminLogin = true;
    }

    //Erros
    auto result = LoginTry::Error;
    if (!isFirstAdminLogin && data.size() < 2) {
        ESP_LOGE(__FUNCTION__, "Dados invalidos");
        SendLoginTryResult(result, connection);
        return;
    }

    std::string user = data[0];
    std::string pw;
    if (data.size() > 1) {
        pw = data[1];
    }

    //Verifica usuario
    if (user == adminUser) {
        if (isFirstAdminLogin || pw == SdCard::LoadConfig(AdminPasswordKey)) {
            connectedUser->IsAdmin = true;
            connectedUser->IsLogged = true;
            connectedUser->User = user;
            connectedUser->IsFirstAdminLogin = isFirstAdminLogin;
            result = LoginTry::Ok;
        } else {
            result = LoginTry::WrongPassword;
        }
    } else {
        result = CheckPassword(user, pw);
        if (result == LoginTry::Ok) {
            connectedUser->IsLogged = true;
            connectedUser->User = user;
            connectedUser->OnLogin();

        }
    }

    SendLoginTryResult(result, connection);
}

void UserManager::SendLoginTryResult(LoginTry loginTry, BluetoothConnection *connection) {
    nlohmann::json j;
    string result;
    switch (loginTry) {
        case LoginTry::Ok:
            result = "Ok";
            break;
        case LoginTry::NotConfirmed:
            result = "NotConfirmed";
            break;
        case LoginTry::Error:
            result = "Error";
            break;
        case LoginTry::WrongPassword:
            result = "WrongPassword";
            break;
    }

    j["Result"] = result;
    auto *user = connection->GetUser();
    if (loginTry == LoginTry::Ok) {
        j["IsAdmin"] = user->IsAdmin;
        j["IsFirstAdminLogin"] = user->IsFirstAdminLogin;
    }

    auto json = j.dump();
    connection->SendJsonData(json);
}


auto UserManager::CheckPassword(string &userName, string &pass) -> LoginTry {
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Tentativa User:%s , Senha:%s", userName.c_str(), pass.c_str());
#endif
    auto user = LoadUser(userName);
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Senha Armazenada :%s", user.Password.c_str());
#endif

    if (user.Name.empty()) {
        return LoginTry::Error;
    }

    if (!user.IsConfirmed) {
        return LoginTry::NotConfirmed;
    }

    if (user.Password != pass) {
        return LoginTry::WrongPassword;
    } else {
        return LoginTry::Ok;
    }
}

void UserManager::Logoff(BluetoothConnection *connection) {
    auto *connectedUser = connection->GetUser();
    connectedUser->Clear();

    nlohmann::json j;
    j["Ok"] = !connectedUser->IsLogged;
    auto json = j.dump();
    connection->SendJsonData(json);
}