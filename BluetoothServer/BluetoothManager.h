//
// Created by maikeu on 12/02/2022.
//

#ifndef BLUETOOTHMANAGER_H
#define BLUETOOTHMANAGER_H


#include "NimBLEDevice.h"
#include "projectConfig.h"
#include "Singleton.h"
#include <sstream>
#include <list>

class BluetoothUtility {
private:
    static std::list<std::string> uuidList;
public:
    static std::string GetUniqueId(bool isCharacteristic);
};

class BluetoothManager : public Singleton<BluetoothManager> {
private:
    NimBLEService *publicService = nullptr;
    NimBLEService *privateService = nullptr;
    NimBLECharacteristic *publicTxCharacteristic = nullptr;
    std::string privateServiceUUID;
public:
    explicit BluetoothManager(token);;

    class ServerCallbacks : public NimBLEServerCallbacks {
        void onConnect(NimBLEServer *server, ble_gap_conn_desc *desc) override;
    };

    class SendDataCallbacks : public NimBLECharacteristicCallbacks {
    public:
        void onRead(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc) override;
    };

    NimBLEServer *BleServer = nullptr;

    void Init() {
        NimBLEDevice::init(DEVICE_NAME);

        // Set power to 9DB
        NimBLEDevice::setPower(ESP_PWR_LVL_P9);

        // Create the BLE Server
        BleServer = NimBLEDevice::createServer();

        // Create the BLE Service
        publicService = BleServer->createService(PUBLIC_SERVICE_UUID);
        auto uuid = BluetoothUtility::GetUniqueId(false);
        privateService = BleServer->createService(uuid);
        privateServiceUUID = privateService->getUUID().toString();
//        ConnectionManager::Init(CONFIG_BT_NIMBLE_MAX_CONNECTIONS);


        // Create a BLE Characteristic
        publicTxCharacteristic = publicService->createCharacteristic(
                PUBLIC_CHARACTERISTIC_TX_UUID,
                NIMBLE_PROPERTY::READ);

        BleServer->setCallbacks(new ServerCallbacks());
        publicTxCharacteristic->setCallbacks(new SendDataCallbacks);

        // Start the service
        publicService->start();
        privateService->start();

        // Start advertising
        NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
        pAdvertising->addServiceUUID(PUBLIC_SERVICE_UUID);
        pAdvertising->setScanResponse(true);
        pAdvertising->setMinPreferred(0x0);
        NimBLEDevice::startAdvertising();

//        Utility::CreateAndProfile("SendDataTask", SendDataTask, 8192, HIGH_PRIORITY, 1, nullptr);
    }
};


#endif //BLUETOOTHMANAGER_H
