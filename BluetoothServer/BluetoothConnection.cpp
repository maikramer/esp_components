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

//#define LOG_SENT
//#define LOG_STATUS_SENT


[[maybe_unused]] void
BluetoothConnection::SetGetDataFunction(std::function<list<uint8_t>()> callback) {
    _getDataFunction = std::move(callback);
}

void BluetoothConnection::SetNotificationNeeds(NotificationNeeds needs) {
    _notificationNeeds = needs;
}

auto BluetoothConnection::GetNotificationNeeds() -> NotificationNeeds {
    return _notificationNeeds;
}

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

void BluetoothConnection::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo)
{
    auto address = connInfo.getIdAddress();
    std::string rxValue = pCharacteristic->getValue();
    if (rxValue.length() > 0) {
        ESP_LOGI(__FUNCTION__, "From Peer %s: %s", address.toString().c_str(), rxValue.c_str());
    }

    auto *connection = ConnectionManager::GetConnectionById(address.getNative()[5]);
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
#ifdef DEVICE_BASED_DATA
    auto list = _user->GetData();
#else
    if (_getDataFunction == nullptr) {
        ESP_LOGE(__FUNCTION__, "Sem funcoes para envio de dados definidas");
        return;
    }
    auto list = _getDataFunction();

    if (list.empty() && !_needsFirstUpdate) {
//        ESP_LOGI(__FUNCTION__, "Nao Enviando");
        return;
    }
    _needsFirstUpdate = true;
#if LOG_SENT
    ESP_LOGI(__FUNCTION__, "Enviando");
#endif
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

void BluetoothConnection::onStatus(NimBLECharacteristic *pCharacteristic, int code)
{
    auto str = NimBLEUtils::returnCodeToString(code);
#ifdef LOG_STATUS_SENT
        ESP_LOGI("Status", "%s", str.c_str());
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

#ifdef USER_MANAGEMENT_ENABLED

void BluetoothConnection::Logoff() {
    _needsFirstUpdate = true;
    _user = nullptr;
}

#endif

void BluetoothConnection::Disconnect() {
    _isFree = true;
    _conn_ID = -1;
    DisconnectEvent.trigger(this, nullptr);

#ifdef USER_MANAGEMENT_ENABLED
    vTaskDelay(500);
    if (_user != nullptr) {
        UserManager::Disconnect(_user);
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

