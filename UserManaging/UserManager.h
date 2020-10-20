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

class BluetoothConnection;

constexpr char AdminUserKey[] = "AdminUser";
constexpr char AdminPasswordKey[] = "AdminPassword";

namespace ErrorCodes {
    const ErrorCodeItem NotConfirmed{"NotConfirmed",
                                     "Usuário precisa de Liberação do Administrador antes de ser Usado",
                                     ErrorCodeType::User};
    const ErrorCodeItem UserNotFound{"UserNotFound",
                                     "Usuário não Encontrado!", ErrorCodeType::User};
    const ErrorCodeItem WrongPassword{"WrongPassword", "Senha Incorreta", ErrorCodeType::User};
    const ErrorCodeItem AdminNotRegistered{"AdminNotRegistered",
                                           "O Administrador não foi cadastrado ainda, o próximo registro será cadastrado como Administrador",
                                           ErrorCodeType::User};
    const ErrorCodeItem NoUsersRegistered{"NoUsersRegistered", "Nenhum Usuário Cadastrado", ErrorCodeType::User};
}

class UserManager {
    friend ConnectedUser;
    using string = std::string;
public:

    static void SetAdmin(const std::vector<std::string> &data, BluetoothConnection *connection);

    void Login(const std::vector<std::string> &data, BluetoothConnection *connection);

    static void Logoff(BluetoothConnection *connection);

    static auto SaveUser(const JsonModels::User &user) -> ErrorCode;

    static ErrorCode LoadUser(const string &userName, JsonModels::User &user);

    static void SignUp(const string &jsonStr, BluetoothConnection *connection);

    static void GetUsersWaitingForApproval(BluetoothConnection *connection);

    static void ApproveUser(const string &userName, BluetoothConnection *pConnection);

    static ErrorCode CheckPassword(string &user, string &pass);

    static void SendLoginTryResult(ErrorCode errorCode, BluetoothConnection *connection);

    void CreateManager();

protected:
    virtual ConnectedUser *CreateUserInstance();

    static SafeList<ConnectedUser *> _activeUsers;//NOLINT
};

#endif

#endif //TOMADA_SMART_CONDO_USERMANAGER_H
