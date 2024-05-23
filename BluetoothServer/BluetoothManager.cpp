//
// Created by maikeu on 12/02/2022.
//

#include <esp_random.h>
#include "BluetoothManager.h"
#include "JsonModels.h"

std::list<std::string> BluetoothUtility::uuidList;

auto BluetoothUtility::GetUniqueId(bool isCharacteristic) -> std::string {
    bool isUnique = false;
    std::string uuidStr;
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

void BluetoothManager::ServerCallbacks::onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo)
{
    NimBLEDevice::startAdvertising();
}

void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
{
    JsonModels::UuidInfoJsonData jsonData{};
//    jsonData.ServiceUUID = BluetoothManager::instance().GetPrivateServiceUUID();
//    jsonData.WriteUUID = GetWriteUUID();
//    jsonData.NotifyUUID = GetNotifyUUID();
//    auto json = jsonData.ToJson();
//    ESP_LOGI(__FUNCTION__, "Sending Json");
//    unsigned char bytes[json.length()];
//    auto size = Utility::StringToByteArray(json, bytes);
//    pCharacteristic->setValue(bytes, size);
//    ESP_LOGI(__FUNCTION__, "Json Sent : %s", json.c_str());
    vTaskDelay(50 / portTICK_PERIOD_MS);
}

BluetoothManager::BluetoothManager(Singleton<BluetoothManager>::token) {}
