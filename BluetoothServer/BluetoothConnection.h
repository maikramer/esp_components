#ifndef TOMADA_SMART_CONDO_BLUETOOTHCONNECTION_H
#define TOMADA_SMART_CONDO_BLUETOOTHCONNECTION_H

#include <string>
#include <custom/BLECharacteristic.h>
#include <ConnectedUser.h>
#include "JsonData.h"

class BluetoothConnection : public BLECharacteristicCallbacks {
public:
    BluetoothConnection(ConnectedUser *user);

    BLECharacteristic *WriteCharacteristic = nullptr;
    BLECharacteristic *NotifyCharacteristic = nullptr;
    SemaphoreHandle_t xSendMutex = xSemaphoreCreateMutex();
    SemaphoreHandle_t xCanNotifySemaphore = xSemaphoreCreateBinary();

    std::string GetWriteUUID() {
        return WriteCharacteristic->getUUID().toString();
    }

    std::string GetNotifyUUID() {
        return NotifyCharacteristic->getUUID().toString();
    }

    void Free() {
        _isFree = false;
        _conn_ID = -1;
        _user->Clear();
    }

    void Setup(uint16_t conn_id) {
        _conn_ID = conn_id;
        _isFree = false;
    }

    bool IsFree() { return _isFree; }

    int GetId() { return _conn_ID; }

    void Init();

    void onWrite(BLECharacteristic *pCharacteristic, uint16_t conn_id) override;

    void onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code) override;

    ConnectedUser *GetUser() { return _user; }

    void SendUsageData(bool isNotification) const;

    std::string GetConnectionInfoJson();

    void SendJsonData(const std::string &json);

private:
    bool _isFree = true;
    int _conn_ID = -1;
    ConnectedUser* _user;

    void SendJson(const std::string &json) const;
};

#endif