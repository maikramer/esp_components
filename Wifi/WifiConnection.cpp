#include "WifiConnection.h"
#include "GeneralErrorCodes.h"
#include "CommunicationErrorCodes.h"
#include <cstring>
#include <esp_log.h>

EventGroupHandle_t WifiConnection::_wifiEventGroup;
const char *WifiConnection::TAG = "WifiConnection";

/**
 * @file WifiConnection.cpp
 * @brief Implementation of the WifiConnection class for managing WiFi connections in station mode.
 */

WifiConnection::WifiConnection() :
        BaseConnection(), _ipAddress(), _retryNum(0),
        _isConnected(false), _connId(0), _wifiClient(nullptr) {}

WifiConnection::~WifiConnection() {
    disconnect();//NOLINT
}

ErrorCode WifiConnection::connect(const std::string &ssid, const std::string &password) {
    _ssid = ssid;
    _password = password;

    // Initialize NVS Flash (required for WiFi)
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    _wifiEventGroup = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    esp_netif_create_default_wifi_sta();

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    // Register event handlers
    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &eventHandler,
                                                        this,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &eventHandler,
                                                        this,
                                                        &instance_got_ip));

    // Configure WiFi connection
    wifi_config_t wifi_config = {};
    std::strncpy((char *) wifi_config.sta.ssid, ssid.c_str(), sizeof(wifi_config.sta.ssid));
    std::strncpy((char *) wifi_config.sta.password, password.c_str(), sizeof(wifi_config.sta.password));

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));

    // Start WiFi
    ESP_ERROR_CHECK(esp_wifi_start());

    ESP_LOGI(TAG, "Connecting to WiFi network: %s", ssid.c_str());

    // Wait for connection or failure
    EventBits_t bits = xEventGroupWaitBits(_wifiEventGroup,
                                           WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
                                           pdFALSE,
                                           pdFALSE,
                                           pdMS_TO_TICKS(10000));

    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "Connected to WiFi network: %s", ssid.c_str());
        return CommonErrorCodes::None;
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGE(TAG, "Failed to connect to WiFi network: %s", ssid.c_str());
        return CommonErrorCodes::WifiConnectionFailed;
    } else {
        ESP_LOGE(TAG, "Timeout waiting for WiFi connection.");
        return CommonErrorCodes::Timeout;
    }
}

void WifiConnection::disconnect() {
    if (isConnected()) {
        ESP_ERROR_CHECK(esp_wifi_disconnect());
        xEventGroupWaitBits(_wifiEventGroup, WIFI_FAIL_BIT, pdFALSE, pdTRUE, portMAX_DELAY);
        ESP_LOGI(TAG, "Disconnected from WiFi network.");
    }
    _isConnected = false;
}

bool WifiConnection::isConnected() const {
    return _isConnected;
}

std::string WifiConnection::getSSID() const {
    if (!isConnected()) {
        return "";
    }

    wifi_config_t wifi_config;
    ESP_ERROR_CHECK(esp_wifi_get_config(WIFI_IF_STA, &wifi_config));
    return {(char *) wifi_config.sta.ssid};
}

IPAddress WifiConnection::getIPAddress() const {
    return _ipAddress;
}

ErrorCode WifiConnection::sendRawData(const uint8_t *data, size_t length) const {
    auto error = _wifiClient->write(data, length);
    if (error != CommonErrorCodes::None) {
        error.log(TAG);
        return error;
    }

    return CommonErrorCodes::None;
}

void WifiConnection::eventHandler(void *arg, esp_event_base_t event_base,
                                  int32_t event_id, void *event_data) {
    auto *self = static_cast<WifiConnection *>(arg);

    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (self->_retryNum < 5) {
            esp_wifi_connect();
            self->_retryNum++;
            ESP_LOGI(TAG, "Retrying connection to WiFi network...");
        } else {
            xEventGroupSetBits(_wifiEventGroup, WIFI_FAIL_BIT);
            self->onDisconnect.trigger(self, nullptr);
            self->_isConnected = false; // Update connection status
        }
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        auto *event = (ip_event_got_ip_t *) event_data;
        self->_ipAddress = IPAddress(event->ip_info.ip);
        self->_retryNum = 0;
        self->_isConnected = true; // Update connection status
        xEventGroupSetBits(_wifiEventGroup, WIFI_CONNECTED_BIT);
        // You can trigger an onConnect event here if needed
    }
}