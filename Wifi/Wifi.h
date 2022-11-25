//
// Created by maikeu on 22/06/22.
//

#ifndef WIFI_H
#define WIFI_H

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_event.h>
#include <string>
#include <esp_wifi_types.h>
#include <esp_wifi_default.h>
#include <esp_wifi.h>
#include <cstring>
#include <esp_log.h>
#include <Event.h>
#include "BluetoothConnection.h"
#include "WirelessDevice.h"
#include "Commander.h"
#include "CommandCode.h"

constexpr auto WIFI_CONNECTED_BIT = BIT0;
constexpr auto WIFI_FAIL_BIT = BIT1;
constexpr auto DEFAULT_SCAN_LIST_SIZE = 32;
enum class WifiState {
    Connected,
    Disconnected,
    GotIp
};

namespace ErrorCodes {
    const ErrorCodeItem NotDisconnectedToScan{"NotDisconnectedToScan",
                                              "Deve estar Desconectado para usar o Scan",
                                              ErrorCodeType::Communication};
}
namespace MK {
    class Wifi {
    public:
        static void Start(std::string ssid, std::string password);

        static void Init() {
            ErrorCode::AddErrorItem(ErrorCodes::NotDisconnectedToScan);
            ESP_ERROR_CHECK(esp_netif_init());
            ESP_ERROR_CHECK(esp_event_loop_create_default());
            esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
            assert(sta_netif);
            const DeviceCommand ScanForWifi(0, std::string(NAMEOF(ScanForWifi)),
                                            (uint8_t) CommandCode::ScanForWifi,
                                            MK::Wifi::ScanAndSend);
            Commander::AddCommand(ScanForWifi);
        }

        static void ScanAndSend(const std::vector<std::string> &data,
                                BluetoothConnection *connection) {
            ESP_LOGI(__FUNCTION__, "Iniciando Escaneamento");
            std::map<std::string, WirelessDevice> map{};
            if (_state != WifiState::Disconnected) {
                ESP_LOGE(__FUNCTION__, "Cuidado, você não está desconectado!!");
                connection->SendError<ScanForWifiListJsonData>(ErrorCodes::NotDisconnectedToScan);
                return;
            }


            wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
            ESP_ERROR_CHECK(esp_wifi_init(&cfg));

            uint16_t number = DEFAULT_SCAN_LIST_SIZE;
            wifi_ap_record_t ap_info[DEFAULT_SCAN_LIST_SIZE];
            uint16_t ap_count = 0;
            memset(ap_info, 0, sizeof(ap_info));

            ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
            ESP_ERROR_CHECK(esp_wifi_start());
            esp_wifi_scan_start(NULL, true);
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_records(&number, ap_info));
            ESP_ERROR_CHECK(esp_wifi_scan_get_ap_num(&ap_count));
            ESP_LOGI(__FUNCTION__, "Total APs scanned = %u", ap_count);
            for (int i = 0; (i < DEFAULT_SCAN_LIST_SIZE) && (i < ap_count); i++) {
                ESP_LOGI(__FUNCTION__, "SSID \t\t%s", ap_info[i].ssid);
                ESP_LOGI(__FUNCTION__, "RSSI \t\t%d", ap_info[i].rssi);
                ESP_LOGI(__FUNCTION__, "Channel \t\t%d\n", ap_info[i].primary);
                WirelessDevice device{(char *) ap_info[i].ssid, ap_info[i].rssi,
                                      ap_info[i].primary};
                map[device.Ssid] = device;
            }
            connection->SendList<ScanForWifiListJsonData>(map);

        };
        static NakedEvent OnGotIp;
        static NakedEvent OnConnected;
        static NakedEvent OnDisconnected;

        static WifiState GetState() {
            return _state;
        }

    private:
        static WifiState _state;
        static int _retry_num;
        static int _maxRetries;
        static EventGroupHandle_t s_wifi_event_group;

        static void Wifi_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id,
                                       void *event_data);

        static std::string _ssid;
        static std::string _password;
    };
}


#endif //WIFI_H
