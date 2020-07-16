#ifndef TOMADA_SMART_CONDO_BLUETOOTHCONNECTION_H
#define TOMADA_SMART_CONDO_BLUETOOTHCONNECTION_H

#include <string>
#include <custom/BLECharacteristic.h>
#include <functional>
#include "projectConfig.h"

#ifdef USER_MANAGEMENT_ENABLED

class ConnectedUser;

#endif

#include "JsonData.h"
#include "list"

enum class NotificationNeeds {
    NoSend,
    SendNormal,
    SendImportant
};

class BluetoothConnection : public BLECharacteristicCallbacks {
public:
#ifdef USER_MANAGEMENT_ENABLED

    BluetoothConnection(ConnectedUser *user);

#endif
    BLECharacteristic *WriteCharacteristic = nullptr;
    BLECharacteristic *NotifyCharacteristic = nullptr;
    SemaphoreHandle_t xSendMutex = xSemaphoreCreateMutex();

    [[nodiscard]] std::string GetWriteUUID() const;

    [[nodiscard]] std::string GetNotifyUUID() const;

    void Free();

    void Setup(uint16_t conn_id);

    [[nodiscard]] bool IsFree() const;

    [[nodiscard]] int GetId() const;

    void Init();

    void onWrite(BLECharacteristic *pCharacteristic, uint16_t conn_id) override;

    void onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code) override;
    void SendSimpleResult(uint8_t errorCode);

#ifdef USER_MANAGEMENT_ENABLED

    ConnectedUser *GetUser() { return _user; }

#else
    NotificationNeeds GetNotificationNeeds();
    void SetGetDataFunction(std::function<std::list<uint8_t>()> callback);

    void SetNotificationNeeds(NotificationNeeds needs);
#endif

    void SendNotifyData(bool isNotification);

    std::string GetConnectionInfoJson() const;

    void SendJsonData(const std::string &json);

private:
    bool _isFree = true;
    int _conn_ID = -1;
#ifdef USER_MANAGEMENT_ENABLED
    ConnectedUser *_user;
#else
    std::function<std::list<uint8_t> ()> _getDataFunction;
#endif

    void SendJson(const std::string &json) const;


    NotificationNeeds _notificationNeeds = NotificationNeeds::NoSend;
    };

#endif