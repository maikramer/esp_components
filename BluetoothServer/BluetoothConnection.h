#ifndef TOMADA_SMART_CONDO_BLUETOOTHCONNECTION_H
#define TOMADA_SMART_CONDO_BLUETOOTHCONNECTION_H

#include <string>
#include <custom/BLECharacteristic.h>
#include <functional>
#include <JsonModels.h>
#include <Storage.h>
#include "projectConfig.h"
#include "ErrorCode.h"

#ifdef USER_MANAGEMENT_ENABLED

class ConnectedUser;

#endif

#include "list"

enum class NotificationNeeds {
    NoSend,
    SendNormal,
    SendImportant
};

class BluetoothConnection : public BLECharacteristicCallbacks {
public:
#ifdef USER_MANAGEMENT_ENABLED

    explicit BluetoothConnection(ConnectedUser *user);

#endif
    BLECharacteristic *WriteCharacteristic = nullptr;
    BLECharacteristic *NotifyCharacteristic = nullptr;
    SemaphoreHandle_t xSendMutex = xSemaphoreCreateMutex();

    [[nodiscard]] std::string GetWriteUUID() const;

    [[nodiscard]] std::string GetNotifyUUID() const;

    template<typename Tmodel>
    void SendError(ErrorCode errorCode) {
        static_assert(std::is_base_of<JsonModels::BaseJsonData, Tmodel>::value,
                      "Lista deve ter como base BaseListJsonData");
        nlohmann::json j;

        Tmodel jsonData;
        jsonData.ErrorCode = (uint8_t) errorCode;
        if (std::is_base_of<JsonModels::BaseListJsonDataBasic, Tmodel>()) {
            reinterpret_cast<JsonModels::BaseListJsonDataBasic *>(&jsonData)->End = true;
        }

        auto json_str = jsonData.ToJson();
        SendJsonData(json_str);
        ESP_LOGW(__FUNCTION__, "Erro lendo lista");
    }

    template<typename Tmodel, typename T1, typename T2>
    void SendList(const std::map<T1, T2> &map) {
        static_assert(std::is_base_of<JsonModels::BaseListJsonDataBasic, Tmodel>::value,
                      "Lista deve ter como base BaseListJsonData");

        if (map.empty()) {
            SendError<Tmodel>(ErrorCode(ErrorCodes::ListIsEmpty));
            return;
        }

        for (auto it = map.begin(); it != map.end(); ++it) {
            Tmodel jsonData;
            if (it == map.begin()) {
                jsonData.Begin = true;
            }

            jsonData.FromPair(it->first, it->second);

            if (std::next(it) == map.end()) {
                jsonData.End = true;
            }
            auto jr_str = jsonData.ToJson();
//#ifdef LOGGING
            ESP_LOGI(__FUNCTION__, "Sending %s", jr_str.c_str());
//#endif
            SendJsonData(jr_str);
        }
    }

    void Free();

    void Setup(uint16_t conn_id);

    [[nodiscard]] bool IsFree() const;

    [[nodiscard]] int GetId() const;

    void Init();

    void onWrite(BLECharacteristic *pCharacteristic, uint16_t conn_id) override;

    void onStatus(BLECharacteristic *pCharacteristic, Status s, uint32_t code) override;

#ifdef USER_MANAGEMENT_ENABLED

    ConnectedUser *GetUser() { return _user; }

#else
    NotificationNeeds GetNotificationNeeds();
    void SetGetDataFunction(std::function<std::list<uint8_t>()> callback);

    void SetNotificationNeeds(NotificationNeeds needs);
#endif

    void SendNotifyData(bool isNotification);

    [[nodiscard]] std::string GetConnectionInfoJson() const;

    void SendJsonData(const std::string &json);

private:
    bool _isFree = true;
    int _conn_ID = -1;
#ifdef USER_MANAGEMENT_ENABLED
    ConnectedUser *_user;
#else
    std::function<std::list<uint8_t> ()> _getDataFunction;
#endif

    void SendJson(const std::string &json) const;


    NotificationNeeds _notificationNeeds = NotificationNeeds::NoSend;
};

#endif