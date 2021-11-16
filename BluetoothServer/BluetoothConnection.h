#ifndef TOMADA_SMART_CONDO_BLUETOOTHCONNECTION_H
#define TOMADA_SMART_CONDO_BLUETOOTHCONNECTION_H

#include <string>
#include <NimBLECharacteristic.h>
#include <functional>
#include <JsonModels.h>
#include <Storage.h>
#include "projectConfig.h"
#include "ErrorCode.h"
#include "Event.h"

#ifdef USER_MANAGEMENT_ENABLED

class ConnectedUser;

#endif

#include "list"

enum class NotificationNeeds {
    NoSend,
    SendNormal,
    SendImportant
};

class BluetoothConnection : public NimBLECharacteristicCallbacks {
public:
    NimBLECharacteristic *WriteCharacteristic = nullptr;
    NimBLECharacteristic *NotifyCharacteristic = nullptr;
    SemaphoreHandle_t xSendMutex = nullptr;
    Event<BluetoothConnection *, void *> DisconnectEvent;

    [[nodiscard]] std::string GetWriteUUID() const;

    [[nodiscard]] std::string GetNotifyUUID() const;

    template<typename Tmodel>
    void SendError(ErrorCode errorCode) {
        static_assert(std::is_base_of<JsonModels::BaseJsonDataError, Tmodel>::value,
                      "Lista deve ter como base BaseListJsonData");

        if (errorCode != ErrorCodes::None) {
            ESP_LOGE(__FUNCTION__, "Erro \"%s\" -> %s", errorCode.GetName(),
                     errorCode.GetDescription());
        }

        Tmodel jsonData;
        jsonData.ErrorMessage = errorCode;
        if (std::is_base_of<JsonModels::BaseListJsonDataBasic, Tmodel>() ||
            std::is_same<JsonModels::BaseListJsonDataBasic, Tmodel>()) {
            reinterpret_cast<JsonModels::BaseListJsonDataBasic *>(&jsonData)->End = true;
        }

        auto json_str = jsonData.ToJson();
        SendJsonData(json_str);
    }

    template<typename Tmodel, typename T1, typename T2>
    void
    SendList(const std::map<T1, T2> &map, Tmodel *firstItem = nullptr, Tmodel *lastItem = nullptr) {
        static_assert(std::is_base_of<JsonModels::BaseListJsonDataBasic, Tmodel>::value,
                      "Lista deve ter como base BaseListJsonData");
        if (map.empty()) {
            ErrorCode error = ErrorCodes::ListIsEmpty;
            SendError<Tmodel>(error);
            return;
        }

        for (auto it = map.begin(); it != map.end(); ++it) {
            if (it == map.begin() && firstItem != nullptr) {
                SendJsonData(firstItem->ToJson());
                continue;
            } else if (std::next(it) == map.end() && lastItem != nullptr) {
                SendJsonData(lastItem->ToJson());
                continue;
            }

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

    template<typename Tmodel>
    void SendJsonData(Tmodel jsonData) {
        static_assert(std::is_base_of<JsonModels::BaseJsonDataError, Tmodel>::value,
                      "Lista deve ter como base BaseListJsonData");
        if (std::is_base_of<JsonModels::BaseListJsonDataBasic, Tmodel>()) {
            reinterpret_cast<JsonModels::BaseListJsonDataBasic *>(&jsonData)->End = true;
        }

        auto json_str = jsonData.ToJson();
        SendJsonData(json_str);
    }

    void Disconnect();

    void Connect(uint16_t conn_id);

    [[nodiscard]] bool IsFree() const;

    [[nodiscard]] int GetId() const;

    void Init();

    void onWrite(NimBLECharacteristic *pCharacteristic, ble_gap_conn_desc *desc) override;

    void onStatus(NimBLECharacteristic *pCharacteristic, Status s, int code) override;

#ifdef USER_MANAGEMENT_ENABLED

    ConnectedUser *GetUser(bool canBeNull, bool canBeEmpty);

    void SetUser(ConnectedUser *user) {
        _user = user;
    }

#else

    NotificationNeeds GetNotificationNeeds();

    void SetGetDataFunction(std::function<std::list<uint8_t>()> callback);

    void SetNotificationNeeds(NotificationNeeds needs);

#endif

    void SendNotifyData(bool isNotification);

    [[nodiscard]] std::string GetConnectionInfoJson() const;

    void SendJsonData(const std::string &json);

    void Test();

private:
    bool _isFree = true;
    int _conn_ID = -1;
#ifdef USER_MANAGEMENT_ENABLED
    ConnectedUser *_user;
#else
    std::function<std::list<uint8_t>()> _getDataFunction;
#endif

    void SendJson(const std::string &json) const;


    NotificationNeeds _notificationNeeds = NotificationNeeds::NoSend;
    Status _lastStatus = SUCCESS_NOTIFY;
    bool _indicateFailed = false;


};

#endif