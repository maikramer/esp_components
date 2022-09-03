
//
// Created by maikeu on 14/08/2019.
//
#include <NimBLECharacteristic.h>
#include <NimBLEServer.h>

#include <cstdint>
#include <cstdio>
#include <esp_log.h>
#include "BluetoothServer.h"
#include "priorities.h"
#include "ConnectionManager.h"
#include <sstream>
#include <Utility.h>
#include <utility>
#include <ConnectedUser.h>
#include <esp_random.h>

static void SendDataTask(void *arg __unused) {
    TickType_t xLastWakeTime = 0;
    xLastWakeTime = xTaskGetTickCount();
    for (;;) {
        ConnectionManager::SendNotifications();

        vTaskDelayUntil(&xLastWakeTime, SEND_DATARATE);
    }
}

BluetoothServer::BluetoothServer(token) {
    ErrorCode::AddErrorItem(ErrorCodes::JsonError);
    ErrorCode::AddErrorItem(ErrorCodes::CommunicationError);
}

//Cuidado, se usar mais de uma vez pode haver conflito.
auto BluetoothServer::CreatePrivateService() -> NimBLEService * {
    auto uuid = GetUniqueId(false);
    //    ESP_LOGI(__FUNCTION__, "Criado servico com UUID %s", uuidStr.c_str());
    return BleServer->createService(uuid);
}

auto BluetoothServer::CreateWriteCharacteristic(NimBLEService *service) -> NimBLECharacteristic * {
    auto uuid = GetUniqueId(true);

    if (service == nullptr)
        service = publicService;

    auto *ch = service->createCharacteristic(
            uuid, NIMBLE_PROPERTY::WRITE);

    return ch;
}

auto BluetoothServer::CreateNotifyCharacteristic(NimBLEService *service) -> NimBLECharacteristic * {
    auto uuid = GetUniqueId(true);

    if (service == nullptr)
        service = publicService;

    auto *ch = service->createCharacteristic(
            uuid, NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE);

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
        for (const auto &item: uuidList) {
            if (item == uuidStr) {
                isUnique = false;
            }
        }
    }

    uuidList.push_back(uuidStr);
    return uuidStr;
}

void BluetoothServer::SetupBt(std::string deviceName) {

    // Create the BLE Device
    NimBLEDevice::init(std::move(deviceName));
    // Set power to 9DB
    NimBLEDevice::setPower(ESP_PWR_LVL_P9);

    // Create the BLE Server
    BleServer = NimBLEDevice::createServer();
    // Create the BLE Service
    publicService = BleServer->createService(PUBLIC_SERVICE_UUID);
    privateService = CreatePrivateService();
    privateServiceUUID = privateService->getUUID().toString();
    ConnectionManager::Init(CONFIG_BT_NIMBLE_MAX_CONNECTIONS);

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

    Utility::CreateAndProfile("SendDataTask", SendDataTask, 8192, HIGH_PRIORITY, 1, nullptr);
}

void BluetoothServer::SendJsonData(NimBLECharacteristic *pCharacteristic, const string &json) {
    if (!json.empty()) {
        SendJson(pCharacteristic, json);
    } else {
        ESP_LOGE(__FUNCTION__, "Json Vazio");
    }
}

void BluetoothServer::SendJson(NimBLECharacteristic *pCharacteristic, const string &json) {
    ESP_LOGI(__FUNCTION__, "Sending Json");
    unsigned char bytes[json.length()];
    auto size = Utility::StringToByteArray(json, bytes);
    pCharacteristic->setValue(bytes, size);
    ESP_LOGI(__FUNCTION__, "Json Sent : %s", json.c_str());
    vTaskDelay(50 / portTICK_PERIOD_MS);
}

void BluetoothServer::ServerCallbacks::onConnect(NimBLEServer *server __unused,
                                                 ble_gap_conn_desc *desc) {
    NimBLEDevice::startAdvertising();
    ESP_LOGI(__FUNCTION__, "Conectado ao Peer %d:%d:%d:%d:%d:%d", desc->peer_id_addr.val[0],
             desc->peer_id_addr.val[1], desc->peer_id_addr.val[2], desc->peer_id_addr.val[3],
             desc->peer_id_addr.val[4], desc->peer_id_addr.val[5]);
    //todo: Resolver a questao da multi conexão
    ConnectionManager::Connect(desc->peer_id_addr.val[5]);
}

void BluetoothServer::ServerCallbacks::onDisconnect(BLEServer *server, ble_gap_conn_desc *desc) {

    ESP_LOGI(__FUNCTION__, "Desconectado do Peer %d:%d:%d:%d:%d:%d", desc->peer_id_addr.val[0],
             desc->peer_id_addr.val[1], desc->peer_id_addr.val[2], desc->peer_id_addr.val[3],
             desc->peer_id_addr.val[4], desc->peer_id_addr.val[5]);
    ConnectionManager::Disconnect(desc->peer_id_addr.val[5]);
}


void BluetoothServer::SendDataCallbacks::onRead(NimBLECharacteristic *pCharacteristic,
                                                ble_gap_conn_desc *desc) {
    ESP_LOGI(__FUNCTION__, "Peer %d:%d:%d:%d:%d:%d", desc->peer_id_addr.val[0],
             desc->peer_id_addr.val[1], desc->peer_id_addr.val[2], desc->peer_id_addr.val[3],
             desc->peer_id_addr.val[4], desc->peer_id_addr.val[5]);
    auto *conn = ConnectionManager::GetConnectionById(desc->peer_id_addr.val[5]);
    auto json = conn->GetConnectionInfoJson();
    SendJsonData(pCharacteristic, json);
}