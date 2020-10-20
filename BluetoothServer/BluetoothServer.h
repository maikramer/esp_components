//
// Created by maikeu on 14/08/2019.
//

#ifndef TOMADA_SMART_CONDO_BLUETOOTHSERVER_H
#define TOMADA_SMART_CONDO_BLUETOOTHSERVER_H

#include <custom/BLECharacteristic.h>
#include <custom/BLEServer.h>
#include <custom/FreeRTOS.h>

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include "Commander.h"
#include <Enums.h>
#include <list>
#include <projectConfig.h>
#include "Singleton.h"
class BluetoothConnection;

#define DELAY500MS (pdMS_TO_TICKS( 500))
#define SEND_DATARATE DELAY500MS

using std::list;
using std::string;

class BluetoothServer : public Singleton<BluetoothServer>{
public:
    BLEServer *BleServer = nullptr;
    explicit BluetoothServer(token);

#ifdef USER_MANAGEMENT_ENABLED
    void SetupBt(ConnectedUser* userType, std::string deviceName);
#else
        void SetupBt(std::string deviceName);
#endif

    BLECharacteristic *CreatePrivateWriteCharacteristic() {
        return CreateWriteCharacteristic(privateService);
    }

    BLECharacteristic *CreatePrivateNotifyCharacteristic() {
        return CreateNotifyCharacteristic(privateService);
    }

    auto CreateWriteCharacteristic(BLEService *service) -> BLECharacteristic *;

    auto CreateNotifyCharacteristic(BLEService *service) -> BLECharacteristic *;

    auto CreatePrivateService() -> BLEService *;

    auto GetPrivateServiceUUID() -> string { return privateServiceUUID; }

private:
    static void SendJson(BLECharacteristic *pCharacteristic, const string &json);

    static void SendJsonData(BLECharacteristic *pCharacteristic, const string &json);

    string privateServiceUUID;
    std::list<std::string> uuidList;

    class ServerCallbacks : public BLEServerCallbacks {
        void onConnect(BLEServer *server, esp_ble_gatts_cb_param_t *param) override;

        void onDisconnect(BLEServer *server, esp_ble_gatts_cb_param_t *param) override;
    };

    class SendDataCallbacks : public BLECharacteristicCallbacks {
    public:
        void onRead(BLECharacteristic *pCharacteristic, uint16_t conn_id) override;
    };

    BLEService *publicService = nullptr;
    BLEService *privateService = nullptr;
    BLECharacteristic *publicTxCharacteristic = nullptr;
    SemaphoreHandle_t xSendDataSemaphore = nullptr;

    string GetUniqueId(bool isCharacteristic);


};

#endif //TOMADA_SMART_CONDO_BLUETOOTHSERVER_H
