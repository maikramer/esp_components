//
// Created by maikeu on 25/09/2019.
//

#ifndef TOMADA_SMART_CONDO_USERMANAGER_H
#define TOMADA_SMART_CONDO_USERMANAGER_H

#include "projectConfig.h"
#ifdef USER_MANAGEMENT_ENABLED

#include <string>
#include <vector>
#include <list>
#include <map>
#include "BluetoothServer.h"
#include <JsonModels.h>
#include <Enums.h>
#include <Storage.h>
#include <SafeList.h>
#include "ErrorCode.h"

#define NUMERO_DE_SLOTS 4

class BluetoothConnection;

constexpr char AdminUserKey[] = "AdminUser";
constexpr char AdminPasswordKey[] = "AdminPassword";

class UserManager {
    using string = std::string;
public:

    static auto GetAdminInfoJson() -> std::string;

    static void SetAdmin(const std::vector<std::string> &data, BluetoothConnection *connection);

    static void Init();

    static void Login(const std::vector<std::string> &data, BluetoothConnection *connection);

    static void Logoff(BluetoothConnection *connection);

    static auto SaveUser(const JsonModels::User& user) -> ErrorCode;

    static auto LoadUser(const string &userName) -> JsonModels::User;

    static void SignUp(const string &jsonStr, BluetoothConnection *connection);

    static void GetUsersWaitingForApproval(BluetoothConnection *connection);

    static void ApproveUser(const std::string &userName);

    static auto CheckPassword(string &user, string &pass) -> LoginTry;

    static void SendLoginTryResult(LoginTry loginTry, BluetoothConnection *connection);
};

#endif

#endif //TOMADA_SMART_CONDO_USERMANAGER_H
