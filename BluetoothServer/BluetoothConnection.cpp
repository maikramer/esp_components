#include <BluetoothConnection.h>
#include <BluetoothServer.h>
#include <Utility.h>
#include <sstream>
#include "ConnectionManager.h"
#include "EnergyPlug.h"

//#define LOG_SENT

void BluetoothConnection::Init() {
    WriteCharacteristic = BluetoothServer::GetInstance()->CreatePrivateWriteCharacteristic();
    WriteCharacteristic->setCallbacks(this);
    NotifyCharacteristic = BluetoothServer::GetInstance()->CreatePrivateNotifyCharacteristic();
    NotifyCharacteristic->setCallbacks(this);
}

void BluetoothConnection::onWrite(BLECharacteristic *pCharacteristic, uint16_t conn_id) {
    std::string rxValue = pCharacteristic->getValue();
    ESP_LOGI(__FUNCTION__, "Connection ID:%i", conn_id);

    if (rxValue.length() > 0) {
        printf("\nReceived Value: %s\n", rxValue.c_str());
    }

    auto *connection = ConnectionManager::GetConnectionById(conn_id);
    Commander::CheckForCommand(rxValue, connection);
}

auto BluetoothConnection::GetConnectionInfoJson() -> std::string {
    nlohmann::json j;
    j["ServiceUUID"] = BluetoothServer::GetInstance()->GetPrivateServiceUUID();
    j["WriteUUID"] = GetWriteUUID();
    j["NotifyUUID"] = GetNotifyUUID();
    return j.dump();
}

void BluetoothConnection::SendUsageData(bool isNotification) const {
    auto *plug = _user.Plug;
    if (plug == nullptr) {
        ESP_LOGE(__FUNCTION__, "Plug invalido");
        return;
    }

    auto json = plug->GetEnergyUsageJson();
    //            ESP_LOGI(__FUNCTION__, "Sending %s\n", json.c_str());
    unsigned char bytes[json.length()];
    auto size = Utility::StringToByteArray(json, bytes);

    NotifyCharacteristic->setValue(bytes, size);
    NotifyCharacteristic->notify(isNotification);
}

void BluetoothConnection::SendJsonData(const string &json) {
    if (!json.empty()) {
        SendJson(json);
    } else {
        ESP_LOGE(__FUNCTION__, "Json Vazio");
    }
}

void BluetoothConnection::SendJson(const string &json) const {
    xSemaphoreTake(xSendMutex, portMAX_DELAY);

#ifdef LOG_SENT
    ESP_LOGI(__FUNCTION__, "Sending Json");
#endif
    unsigned char bytes[json.length()];
    auto size = Utility::StringToByteArray(json, bytes);
    NotifyCharacteristic->setValue(bytes, size);
    NotifyCharacteristic->notify();
#ifdef LOG_SENT
    ESP_LOGI(__FUNCTION__, "Json Sent : %s", json.c_str());
#endif
    vTaskDelay(pdMS_TO_TICKS(100));

    xSemaphoreGive(xSendMutex);
}

void
BluetoothConnection::onStatus(BLECharacteristic *pCharacteristic, BLECharacteristicCallbacks::Status s, uint32_t code) {
#ifdef LOG_SENT
    string str;
    bool error = false;
    switch (s) {
        case SUCCESS_INDICATE:
            str = "SUCCESS_INDICATE";
            break;
        case SUCCESS_NOTIFY:
            str = "SUCCESS_NOTIFY";
            break;
        case ERROR_INDICATE_DISABLED:
            str = "ERROR_INDICATE_DISABLED";
            error = true;
            break;
        case ERROR_NOTIFY_DISABLED:
            str = "ERROR_NOTIFY_DISABLED";
            error = true;
            break;
        case ERROR_GATT:
            str = "ERROR_GATT";
            error = true;
            break;
        case ERROR_NO_CLIENT:
            str = "ERROR_NO_CLIENT";
            error = true;
            break;
        case ERROR_INDICATE_TIMEOUT:
            str = "ERROR_INDICATE_TIMEOUT";
            error = true;
            break;
        case ERROR_INDICATE_FAILURE:
            str = "ERROR_INDICATE_FAILURE";
            error = true;
            break;
    }
    if (error) {
        ESP_LOGE("Status", "%s", str.c_str());
    } else {
        ESP_LOGI("Status", "%s", str.c_str());
    }
#endif
}

