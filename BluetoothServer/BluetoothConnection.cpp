#include <BluetoothConnection.h>
#include <BluetoothServer.h>
#include <Utility.h>
#include <sstream>
#include <esp_log.h>
#include <ConnectedUser.h>
#include "ConnectionManager.h"
#include "JsonModels.h"

#ifndef USER_MANAGEMENT_ENABLED

#include <utility>

#endif

#define LOG_SENT
//#define LOG_STATUS_SENT
#ifndef USER_MANAGEMENT_ENABLED

void BluetoothConnection::SetGetDataFunction(std::function<list<uint8_t>()> callback) {
    _getDataFunction = std::move(callback);
}

void BluetoothConnection::SetNotificationNeeds(NotificationNeeds needs) {
    _notificationNeeds = needs;

}

auto BluetoothConnection::GetNotificationNeeds() -> NotificationNeeds {
    return _notificationNeeds;
}

#endif

void BluetoothConnection::Init() {
    WriteCharacteristic = BluetoothServer::instance().CreatePrivateWriteCharacteristic();
    WriteCharacteristic->setCallbacks(this);
    NotifyCharacteristic = BluetoothServer::instance().CreatePrivateNotifyCharacteristic();
    NotifyCharacteristic->setCallbacks(this);
    xSendMutex = xSemaphoreCreateMutex();
    if (xSendMutex == nullptr) {
        ESP_LOGE(__FUNCTION__, "Erro criando mutex");
    } else {
        ESP_LOGI(__FUNCTION__, "Mutex criado");
    }
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

auto BluetoothConnection::GetConnectionInfoJson() const -> std::string {
    JsonModels::UuidInfoJsonData jsonData{};
    jsonData.ServiceUUID = BluetoothServer::instance().GetPrivateServiceUUID();
    jsonData.WriteUUID = GetWriteUUID();
    jsonData.NotifyUUID = GetNotifyUUID();
    return jsonData.ToJson();
}

void BluetoothConnection::SendNotifyData(bool isNotification) {
#ifdef USER_MANAGEMENT_ENABLED
    auto list = _user->GetData();
#else
    if (_getDataFunction == nullptr) {
//        ESP_LOGE(__FUNCTION__ , "Sem funcoes para envio de dados definidas");
        return;
    }
    auto list = _getDataFunction();
#endif
    uint8_t data[list.size()];
    std::copy(list.begin(), list.end(), data);

    NotifyCharacteristic->setValue(data, list.size());
    NotifyCharacteristic->notify(isNotification);
    _notificationNeeds = NotificationNeeds::NoSend;
}

void BluetoothConnection::SendJsonData(const string &json) {
    if (!json.empty()) {
        SendJson(json);
    } else {
        ESP_LOGE(__FUNCTION__, "Json Vazio");
    }
}

void BluetoothConnection::Test() {
    xSemaphoreTake(xSendMutex, portMAX_DELAY);
    xSemaphoreGive(xSendMutex);
}

void BluetoothConnection::SendJson(const string &json) const {
    xSemaphoreTake(xSendMutex, portMAX_DELAY);

#ifdef LOG_SENT
    ESP_LOGI(__FUNCTION__, "Sending Json");
    ESP_LOGI(__FUNCTION__, "Enviando %s", json.c_str());
#endif

    unsigned char bytes[json.length() + 1];
    auto size = Utility::StringToByteArray(json, bytes);
    NotifyCharacteristic->setValue(bytes, size);
    NotifyCharacteristic->notify();
#ifdef LOG_SENT
    ESP_LOGI(__FUNCTION__, "Json Sent : %s", json.c_str());
#endif
    vTaskDelay(pdMS_TO_TICKS(2 * json.length()));
    xSemaphoreGive(xSendMutex);
}

void
BluetoothConnection::onStatus(BLECharacteristic *pCharacteristic, BLECharacteristicCallbacks::Status s, uint32_t code) {
    _lastStatus = s;

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
            _indicateFailed = true;
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
            _indicateFailed = true;
            error = true;
            break;
        case ERROR_INDICATE_FAILURE:
            str = "ERROR_INDICATE_FAILURE";
            _indicateFailed = true;
            error = true;
            break;
    }
    if (error) {
        ESP_LOGE("Status", "%s", str.c_str());
    }
#ifdef LOG_STATUS_SENT
    else {
        ESP_LOGI("Status", "%s", str.c_str());
    }
#endif
}

auto BluetoothConnection::IsFree() const -> bool { return _isFree; }

auto BluetoothConnection::GetId() const -> int { return _conn_ID; }

void BluetoothConnection::Connect(uint16_t conn_id) {
    _conn_ID = conn_id;
    _isFree = false;
}

auto BluetoothConnection::GetWriteUUID() const -> std::string {
    return WriteCharacteristic->getUUID().toString();
}

auto BluetoothConnection::GetNotifyUUID() const -> std::string {
    return NotifyCharacteristic->getUUID().toString();
}

void BluetoothConnection::Disconnect() {
    _isFree = true;
    _conn_ID = -1;

#ifdef USER_MANAGEMENT_ENABLED
    if (_user != nullptr) {
        _user->Disconnect();
        _user = nullptr;
    }

#else
    _getDataFunction = nullptr;
#endif
}

#ifdef USER_MANAGEMENT_ENABLED
ConnectedUser *BluetoothConnection::GetUser(bool canBeNull, bool canBeEmpty) {

    if (_user == nullptr) {
        if (!canBeNull) ESP_LOGE(__FUNCTION__, "Retornando um usuario nulo!!");
    } else if (_user->User.empty() && !canBeEmpty) {
        ESP_LOGE(__FUNCTION__, "Retornando um usuario vazio!!");
    }
    return _user;
}
#endif

