//
// Created by maikeu on 25/09/2019.
//

#ifndef TOMADA_SMART_CONDO_CONNECTEDUSER_H
#define TOMADA_SMART_CONDO_CONNECTEDUSER_H

#include "projectConfig.h"
#include "Event.h"

#ifdef USER_MANAGEMENT_ENABLED

#include <cstdint>
#include <string>
#include <utility>
#include <Storage.h>
#include <SdCard.h>
#include <BluetoothConnection.h>
#include "Enums.h"
#include "list"
#include "UserManager.h"

class ConnectedUser {
    friend BluetoothConnection;
public:
    virtual ~ConnectedUser();

    std::string User;
    bool IsLogged = false;
    bool IsAdmin = false;
    bool IsContinuingToUse = false;

    auto operator==(const ConnectedUser &other) -> bool {
        return this == &other;
    }

    virtual std::list<uint8_t> GetData();

    virtual NotificationNeeds GetNotificationNeeds();

    virtual ErrorCode Login(bool isAdmin, std::string userName) {
        LoginEvent.FireEvent(this, nullptr);
        IsLogged = true;
        User = std::move(userName);
        return ErrorCodes::None;
    }

    Event<ConnectedUser *, void *> LogoffEvent;
    Event<ConnectedUser *, void *> LoginEvent;
    Event<ConnectedUser *, void *> DisconnectEvent;
    Event<ConnectedUser *, void *> StopEvent;

    virtual void Disconnect() {
        DisconnectEvent.FireEvent(this, nullptr);
        if (!_isLocked) {
            delete (this);
        } else {
            StopSending();
        }
    }

    virtual void Logoff() {
        LogoffEvent.FireEvent(this, nullptr);
        if (!_isLocked) {
            delete (this);
        } else {
            StopSending();
            IsLogged = false;
        }
    }

    virtual void Stop() {
        StopEvent.FireEvent(this, nullptr);
        if (!_isLocked) {
            IsContinuingToUse = false;
        } else {
            ESP_LOGE(__FUNCTION__, "Tentando para um usuario bloqueado");
        }
        StopSending();
    }

    virtual bool Lock() {
        if (_isLocked) {
            return false;
        } else {
            _isLocked = true;
            return true;
        }
    }

    virtual bool Unlock() {
        if (_isLocked) {
            _isLocked = false;
            return true;
        } else {
            return false;
        }
    }

    virtual void StartSending() { _isSendingUpdates = true; }

    virtual void StopSending() { _isSendingUpdates = false; }

    [[nodiscard]] bool IsLocked() const { return _isLocked; }

    ConnectedUser();

protected:
    bool _isLocked = false;
    bool _isSendingUpdates = false;
};

#endif //USER_MANAGEMENT_ENABLED

#endif //TOMADA_SMART_CONDO_CONNECTEDUSER_H
