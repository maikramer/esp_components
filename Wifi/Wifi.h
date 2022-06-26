//
// Created by maikeu on 22/06/22.
//

#ifndef WIFI_H
#define WIFI_H

#include <freertos/FreeRTOS.h>
#include <freertos/event_groups.h>
#include <esp_event.h>
#include <string>
#include <esp_wifi_default.h>
#include <esp_wifi.h>
#include <cstring>
#include <esp_log.h>
#include <Event.h>

constexpr auto WIFI_CONNECTED_BIT = BIT0;
constexpr auto WIFI_FAIL_BIT = BIT1;
enum class WifiState{
    Connected,
    Disconnected,
    GotIp
};
namespace MK {
    class Wifi {
    public:
        static void Start(std::string ssid, std::string password);

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
    };
}


#endif //WIFI_H
