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
                                                        IP_EVENT_STA_GOT_IPV4,
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

int WifiConnection::scan(wifi_ap_record_t* ap_list, uint16_t max_aps) {
    if (ap_list == nullptr || max_aps == 0) {
        ESP_LOGE(TAG, "Parâmetros inválidos para scan");
        return -1;
    }
    
    ESP_LOGI(TAG, "Iniciando scan WiFi...");
    
    // Desconectar se estiver conectando/conectado para permitir scan
    if (_isConnected) {
        esp_wifi_disconnect();
        vTaskDelay(pdMS_TO_TICKS(200));
    }
    
    // Garantir que o WiFi esteja iniciado antes de fazer scan
    esp_err_t ret = esp_wifi_start();
    if (ret != ESP_OK && ret != ESP_ERR_WIFI_NOT_INIT) {
        ESP_LOGW(TAG, "esp_wifi_start() retornou: %s (continuando mesmo assim)", esp_err_to_name(ret));
    }
    
    // Aguardar um pouco para o WiFi inicializar completamente (se necessário)
    vTaskDelay(pdMS_TO_TICKS(200));
    
    // Configurar scan
    wifi_scan_config_t scan_config = {};
    scan_config.ssid = nullptr;  // Escanear todas as redes
    scan_config.bssid = nullptr;
    scan_config.channel = 0;  // Todos os canais
    scan_config.show_hidden = false;
    scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    scan_config.scan_time.active.min = 100;  // 100ms mínimo
    scan_config.scan_time.active.max = 300;  // 300ms máximo
    
    // Iniciar scan
    ret = esp_wifi_scan_start(&scan_config, true);  // true = bloquear até completar
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao iniciar scan: %s", esp_err_to_name(ret));
        return -1;
    }
    
    // Aguardar conclusão do scan
    uint16_t ap_count = 0;
    ret = esp_wifi_scan_get_ap_num(&ap_count);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao obter número de APs: %s", esp_err_to_name(ret));
        return -1;
    }
    
    ESP_LOGI(TAG, "Encontradas %d redes WiFi", ap_count);
    
    // Limitar ao máximo solicitado
    if (ap_count > max_aps) {
        ap_count = max_aps;
    }
    
    // Obter lista de APs
    ret = esp_wifi_scan_get_ap_records(&ap_count, ap_list);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao obter lista de APs: %s", esp_err_to_name(ret));
        return -1;
    }
    
    ESP_LOGI(TAG, "Scan concluído, retornando %d redes", ap_count);
    return static_cast<int>(ap_count);
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
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IPV4) {
        auto *event = (ip_event_got_ipv4_t *) event_data;
        self->_ipAddress = IPAddress(event->ip_info.ip);
        self->_retryNum = 0;
        self->_isConnected = true; // Update connection status
        xEventGroupSetBits(_wifiEventGroup, WIFI_CONNECTED_BIT);
        self->onConnect.trigger(self, nullptr);
    }
}