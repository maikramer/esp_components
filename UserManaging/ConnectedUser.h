//
// Created by maikeu on 25/09/2019.
//

#ifndef TOMADA_SMART_CONDO_CONNECTEDUSER_H
#define TOMADA_SMART_CONDO_CONNECTEDUSER_H

#include "projectConfig.h"
#ifdef USER_MANAGEMENT_ENABLED

#include <cstdint>
#include <string>
#include <Storage.h>
#include <SdCard.h>
#include <BluetoothConnection.h>
#include "Enums.h"
#include "list"

class ConnectedUser {
public:
    virtual ~ConnectedUser() = default;
    std::string User;
    bool IsLogged = false;
    bool IsAdmin = false;
    bool IsFirstAdminLogin = false;
    bool IsContinuingToUse = false;

    auto operator==(const ConnectedUser &other) -> bool {
        return this == &other;
    }
    virtual std::list<uint8_t> GetData() = 0;
    virtual ConnectedUser* CreateNewInstance() = 0;
    virtual NotificationNeeds GetNotificationNeeds() = 0;
    virtual void OnLogin() = 0;
    virtual void OnImportantSent() =0;

    void Clear();
protected:
    virtual void OnClear() = 0;

};

#endif //USER_MANAGEMENT_ENABLED

#endif //TOMADA_SMART_CONDO_CONNECTEDUSER_H
