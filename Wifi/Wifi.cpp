//
// Created by maikeu on 22/06/22.
//

#include <nvs_flash.h>
#include "Wifi.h"

int  MK::Wifi::_retry_num = 0;
int  MK::Wifi::_maxRetries = 5;
std::string MK::Wifi::_ssid;
std::string MK::Wifi::_password;
NakedEvent MK::Wifi::OnConnected{};
NakedEvent MK::Wifi::OnDisconnected{};
NakedEvent MK::Wifi::OnGotIp{};
WifiState MK::Wifi::_state = WifiState::Disconnected;
EventGroupHandle_t  MK::Wifi::s_wifi_event_group = xEventGroupCreate();

void MK::Wifi::Wifi_event_handler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data) {
    const char *TAG = __FUNCTION__;
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "Tentativa %d de conexão ao SSID: %s", _retry_num, _ssid.c_str());
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (_state == WifiState::Connected) {
            ESP_LOGI(TAG, "Conectado ao SSID: %s", _ssid.c_str());
            OnDisconnected.FireEvent();
            _state = WifiState::Disconnected;
        } else if (_retry_num < _maxRetries) {
            esp_wifi_connect();
            _retry_num++;
            ESP_LOGI(TAG, "Falha ao conectar ao SSID: %s, tentando novamente...", _ssid.c_str());
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
            ESP_LOGI(TAG, "Falha ao conectar ao SSID: %s", _ssid.c_str());
        }
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        OnConnected.FireEvent();
        _state = WifiState::Connected;
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *) event_data;
        ESP_LOGI(TAG, "got ip:" IPSTR, IP2STR(&event->ip_info.ip));
        _retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
        OnGotIp.FireEvent();
        _state = WifiState::GotIp;
    }
}

void MK::Wifi::Start(std::string ssid, std::string password) {
    _retry_num = 0;
    _ssid = ssid;
    _password = password;
    const char *TAG = __FUNCTION__;
    if (ssid.length() >= 32) {
        ESP_LOGE(TAG, "SSID não deve ter mais que 32 caracteres!!");
        return;
    }
    if (password.length() >= 64) {
        ESP_LOGE(TAG, "Senha não deve ter mais que 64 caracteres!!");
        return;
    }

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &Wifi::Wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &Wifi::Wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
            .sta = {.ssid = {0},
                    .password = {0},
                    .scan_method= WIFI_FAST_SCAN,
                    .bssid_set=false,
                    .bssid = {0},
                    .channel=0,
                    .listen_interval=0,
                    .sort_method = WIFI_CONNECT_AP_BY_SIGNAL,
                    .threshold = {.rssi=0, .authmode = WIFI_AUTH_WPA2_PSK},
                    .pmf_cfg = {true, false},
                    .rm_enabled = true,
                    .btm_enabled=true,
                    .mbo_enabled = true,
                    .ft_enabled = true,
                    .owe_enabled= true,
                    .reserved=0,

            },
    };
    memcpy(wifi_config.sta.ssid, ssid.c_str(), ssid.length());
    memcpy(wifi_config.sta.password, password.c_str(), password.length());
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Wifi Inicializado, Heap: %ld", xPortGetFreeHeapSize());
    ESP_LOGI(TAG, "Minimum Available Heap: %lu", xPortGetMinimumEverFreeHeapSize());

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {

    } else if (bits & WIFI_FAIL_BIT) {

    } else {
        ESP_LOGE(TAG, "Evento Inesperado ao tentar conectar");
    }
}
