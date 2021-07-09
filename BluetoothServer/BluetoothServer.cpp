
//
// Created by maikeu on 14/08/2019.
//
#include <BLECharacteristic.h>
#include <BLEServer.h>

#include <cstdint>
#include <cstdio>
#include <BLEDevice.h>
#include <BLE2904.h>
#include <esp_log.h>
#include "BluetoothServer.h"
#include "priorities.h"
#include "ConnectionManager.h"
#include <sstream>
#include <Utility.h>
#include <utility>
#include <ConnectedUser.h>

static void SendDataTask(void *arg __unused) {
    TickType_t xLastWakeTime = 0;
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        ConnectionManager::SendNotifications();

        vTaskDelayUntil(&xLastWakeTime, SEND_DATARATE);
    }
}

BluetoothServer::BluetoothServer(token) : xSendDataSemaphore(xSemaphoreCreateMutex()) {
    ErrorCode::AddErrorItem(ErrorCodes::JsonError);
    ErrorCode::AddErrorItem(ErrorCodes::CommunicationError);
}

//Cuidado, se usar mais de uma vez pode haver conflito.
auto BluetoothServer::CreatePrivateService() -> BLEService * {
    auto uuid = GetUniqueId(false);
    //    ESP_LOGI(__FUNCTION__, "Criado servico com UUID %s", uuidStr.c_str());
    return BleServer->createService(uuid);
}

//auto BluetoothServer::CreatePlugCharacteristic() -> BLECharacteristic * {
//    return CreateNotifyCharacteristic(privateService);
//}

auto BluetoothServer::CreateWriteCharacteristic(BLEService *service) -> BLECharacteristic * {
    auto uuid = GetUniqueId(true);

    if (service == nullptr)
        service = publicService;

    auto *ch = service->createCharacteristic(
            uuid, BLECharacteristic::PROPERTY_WRITE);

    return ch;
}

auto BluetoothServer::CreateNotifyCharacteristic(BLEService *service) -> BLECharacteristic * {
    auto uuid = GetUniqueId(true);

    if (service == nullptr)
        service = publicService;

    auto *ch = service->createCharacteristic(
            uuid, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE);

    return ch;
}

auto BluetoothServer::GetUniqueId(bool isCharacteristic) -> std::string {
    bool isUnique = false;
    string uuidStr;
    while (!isUnique) {
        std::stringstream uuid;
        if (isCharacteristic) {
            uuid << CHARACTERISTIC_UUID_STATE_BASE << esp_random() % 10 << esp_random() % 10;
        } else {
            uuid << SERVICE_UUID_BASE << esp_random() % 10 << esp_random() % 10;
        }
        uuidStr = uuid.str();
        ESP_LOGI(__FUNCTION__, "UUID : %s", uuidStr.c_str());
        isUnique = true;
        for (const auto &item : uuidList) {
            if (item == uuidStr) {
                isUnique = false;
            }
        }
    }

    uuidList.push_back(uuidStr);
    return uuidStr;
}

#ifdef USER_MANAGEMENT_ENABLED

void BluetoothServer::SetupBt(ConnectedUser *userType, std::string deviceName) {
#else

void BluetoothServer::SetupBt(std::string deviceName) {
#endif
    // Create the BLE Device
    BLEDevice::init(std::move(deviceName));

    // Create the BLE Server
    BleServer = BLEDevice::createServer();

    // Create the BLE Service
    publicService = BleServer->createService(PUBLIC_SERVICE_UUID);
    privateService = CreatePrivateService();
    privateServiceUUID = privateService->getUUID().toString();
#ifdef USER_MANAGEMENT_ENABLED
    ConnectionManager::Init(CONFIG_BT_ACL_CONNECTIONS);
    delete userType;
#else
    ConnectionManager::Init(CONFIG_BT_ACL_CONNECTIONS);
#endif


    // Create a BLE Characteristic
    publicTxCharacteristic = publicService->createCharacteristic(
            PUBLIC_CHARACTERISTIC_TX_UUID,
            BLECharacteristic::PROPERTY_READ);

    BleServer->setCallbacks(new ServerCallbacks());
    publicTxCharacteristic->setCallbacks(new SendDataCallbacks);

    // Start the service
    publicService->start();
    privateService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(PUBLIC_SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    BLEDevice::startAdvertising();

    Utility::CreateAndProfile("SendDataTask", SendDataTask, 8192, HIGH_PRIORITY, 1, nullptr);
}

void BluetoothServer::SendJsonData(BLECharacteristic *pCharacteristic, const string &json) {
    if (!json.empty()) {
        SendJson(pCharacteristic, json);
    } else {
        ESP_LOGE(__FUNCTION__, "Json Vazio");
    }
}

void BluetoothServer::SendJson(BLECharacteristic *pCharacteristic, const string &json) {
    ESP_LOGI(__FUNCTION__, "Sending Json");
    unsigned char bytes[json.length()];
    auto size = Utility::StringToByteArray(json, bytes);
    pCharacteristic->setValue(bytes, size);
    ESP_LOGI(__FUNCTION__, "Json Sent : %s", json.c_str());
    vTaskDelay(50 / portTICK_PERIOD_MS);
}

void BluetoothServer::ServerCallbacks::onConnect(BLEServer *server __unused, esp_ble_gatts_cb_param_t *param) {
    BLEDevice::startAdvertising();
    auto conn_id = param->connect.conn_id;
    ConnectionManager::Connect(conn_id);
}

void BluetoothServer::ServerCallbacks::onDisconnect(BLEServer *server) {
    auto conn_id = server->getConnId();
    ESP_LOGI(__FUNCTION__, "Disconnect");
    ConnectionManager::Disconnect(conn_id);
}


void BluetoothServer::SendDataCallbacks::onRead(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param) {
    auto conn_id = param->read.conn_id;
    ESP_LOGI(__FUNCTION__, "Connection ID:%i", conn_id);
    auto *conn = ConnectionManager::GetConnectionById(conn_id);
    auto json = conn->GetConnectionInfoJson();
    SendJsonData(pCharacteristic, json);
}