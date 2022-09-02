//
// Created by maikeu on 14/08/2019.
//

#ifndef BLUETOOTHSERVER_H
#define BLUETOOTHSERVER_H

#include <NimBLEDevice.h>
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

namespace ErrorCodes {
    const ErrorCodeItem JsonError{"JsonError", "Problema com Dados", ErrorCodeType::Communication};
    const ErrorCodeItem CommunicationError{"CommunicationError", "Erro de Comunicação", ErrorCodeType::Communication};
}

class BluetoothServer : public Singleton<BluetoothServer> {
public:
    NimBLEServer *BleServer = nullptr;
    explicit BluetoothServer(token);

#ifdef USER_MANAGEMENT_ENABLED
    void SetupBt(ConnectedUser* userType, std::string deviceName);
#else

    void SetupBt(std::string deviceName);

#endif

    NimBLECharacteristic *CreatePrivateWriteCharacteristic() {
        return CreateWriteCharacteristic(privateService);
    }

    NimBLECharacteristic *CreatePrivateNotifyCharacteristic() {
        return CreateNotifyCharacteristic(privateService);
    }

    auto CreateWriteCharacteristic(NimBLEService *service) -> NimBLECharacteristic *;

    auto CreateNotifyCharacteristic(NimBLEService *service) -> NimBLECharacteristic *;

    auto CreatePrivateService() -> NimBLEService *;

    auto GetPrivateServiceUUID() -> string { return privateServiceUUID; }

private:
    static void SendJson(NimBLECharacteristic *pCharacteristic, const string &json);

    static void SendJsonData(NimBLECharacteristic *pCharacteristic, const string &json);

    string privateServiceUUID;
    std::list<std::string> uuidList;

    class ServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer *server, ble_gap_conn_desc* desc) override;

        void onDisconnect(NimBLEServer *server, ble_gap_conn_desc* desc) override;
    };

    class SendDataCallbacks : public NimBLECharacteristicCallbacks {
    public:
        void onRead(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc* desc) override;
    };

    NimBLEService *publicService = nullptr;
    NimBLEService *privateService = nullptr;
    NimBLECharacteristic *publicTxCharacteristic = nullptr;

    string GetUniqueId(bool isCharacteristic);


};

#endif //BLUETOOTHSERVER_H
