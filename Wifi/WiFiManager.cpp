#include "WiFiManager.h"
#include "IPAddress.h"
#include "ErrorCode.h"
#include "GeneralErrorCodes.h"
#include "Storage.h"
#include "esp_log.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <cstring>
#include <string>

namespace {
constexpr char TAG[] = "WiFiManager";
}

WiFiManager::WiFiManager() : wifi_connection_(nullptr), initialized_(false) {
    wifi_connection_ = new WifiConnection();
    config_.ssid[0] = '\0';
    config_.password[0] = '\0';
}

WiFiManager::~WiFiManager() {
    if (wifi_connection_) {
        delete wifi_connection_;
        wifi_connection_ = nullptr;
    }
}

WiFiManager& WiFiManager::instance() {
    static WiFiManager manager;
    return manager;
}

esp_err_t WiFiManager::init() {
    if (initialized_) {
        return ESP_OK;
    }
    
    ESP_LOGI(TAG, "Inicializando WiFi Manager (usando biblioteca Wifi)...");
    
    // Inicializar Storage (suporta tanto SD quanto NVS)
    ErrorCode storage_err = Storage::initialize();
    if (storage_err != CommonErrorCodes::None) {
        ESP_LOGE(TAG, "Falha ao inicializar Storage: %s", storage_err.description().c_str());
        return ESP_FAIL;
    }
    
    // Inicializar WiFi em modo STA mesmo sem credenciais (necessário para scan)
    // Isso garante que o WiFi esteja pronto para scan mesmo quando não há credenciais salvas
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    
    // Inicializar netif e event loop se ainda não foram inicializados
    esp_err_t netif_ret = esp_netif_init();
    if (netif_ret != ESP_OK && netif_ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Erro ao inicializar netif: %s", esp_err_to_name(netif_ret));
        return ESP_FAIL;
    }
    
    esp_err_t event_ret = esp_event_loop_create_default();
    if (event_ret != ESP_OK && event_ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Erro ao criar event loop: %s", esp_err_to_name(event_ret));
        return ESP_FAIL;
    }
    
    // Criar interface STA se não existir
    esp_netif_create_default_wifi_sta();
    
    // Inicializar WiFi se ainda não foi inicializado
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_err_t wifi_init_ret = esp_wifi_init(&cfg);
    if (wifi_init_ret != ESP_OK && wifi_init_ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Erro ao inicializar WiFi: %s", esp_err_to_name(wifi_init_ret));
        return ESP_FAIL;
    }
    
    // Configurar modo STA (necessário para scan)
    esp_err_t mode_ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (mode_ret != ESP_OK) {
        ESP_LOGE(TAG, "Erro ao configurar modo WiFi: %s", esp_err_to_name(mode_ret));
        return ESP_FAIL;
    }
    
    // Iniciar WiFi (necessário para scan funcionar)
    esp_err_t start_ret = esp_wifi_start();
    if (start_ret != ESP_OK && start_ret != ESP_ERR_INVALID_STATE) {
        ESP_LOGE(TAG, "Erro ao iniciar WiFi: %s", esp_err_to_name(start_ret));
        return ESP_FAIL;
    }
    
    // Tentar carregar credenciais salvas
    esp_err_t load_err = load_credentials();
    if (load_err == ESP_OK && strlen(config_.ssid) > 0) {
        ESP_LOGI(TAG, "Credenciais encontradas: SSID '%s'", config_.ssid);
        // Conectar automaticamente se houver credenciais
        std::string ssid_str(config_.ssid);
        std::string password_str(config_.password);
        ErrorCode err = wifi_connection_->connect(ssid_str, password_str);
        if (err == CommonErrorCodes::None) {
            ESP_LOGI(TAG, "Conectado automaticamente ao WiFi");
            updateIpCache();
        } else {
            ESP_LOGW(TAG, "Falha ao conectar automaticamente: %s", err.description().c_str());
        }
    } else {
        ESP_LOGI(TAG, "Nenhuma credencial encontrada");
    }
    
    initialized_ = true;
    ESP_LOGI(TAG, "WiFi Manager inicializado");
    
    return ESP_OK;
}

esp_err_t WiFiManager::connect(const char* ssid, const char* password) {
    if (ssid == nullptr || strlen(ssid) == 0) {
        ESP_LOGE(TAG, "SSID inválido");
        return ESP_ERR_INVALID_ARG;
    }
    
    // Copiar credenciais
    strncpy(config_.ssid, ssid, sizeof(config_.ssid) - 1);
    config_.ssid[sizeof(config_.ssid) - 1] = '\0';
    
    if (password != nullptr) {
        strncpy(config_.password, password, sizeof(config_.password) - 1);
        config_.password[sizeof(config_.password) - 1] = '\0';
    } else {
        config_.password[0] = '\0';
    }
    
    ESP_LOGI(TAG, "Conectando ao WiFi - SSID: '%s'", config_.ssid);
    
    // Conectar usando WifiConnection
    std::string ssid_str(config_.ssid);
    std::string password_str(config_.password);
    ErrorCode err = wifi_connection_->connect(ssid_str, password_str);
    
    if (err == CommonErrorCodes::None) {
        ESP_LOGI(TAG, "Conectado ao WiFi: %s", config_.ssid);
        save_credentials(); // Salvar credenciais no NVS
        updateIpCache();
        return ESP_OK;
    } else if (err == CommonErrorCodes::Timeout) {
        ESP_LOGW(TAG, "Timeout ao conectar ao WiFi");
        return ESP_ERR_TIMEOUT;
    } else {
        ESP_LOGE(TAG, "Falha ao conectar ao WiFi");
        return ESP_FAIL;
    }
}

esp_err_t WiFiManager::disconnect() {
    if (wifi_connection_) {
        wifi_connection_->disconnect();
        ip_address_[0] = '\0';
    }
    return ESP_OK;
}

bool WiFiManager::is_connected() const {
    if (!wifi_connection_) {
        return false;
    }
    return wifi_connection_->isConnected();
}

const char* WiFiManager::get_ssid() const {
    if (!wifi_connection_ || !is_connected()) {
        return config_.ssid; // Retornar do cache se não conectado
    }
    
    std::string ssid = wifi_connection_->getSSID();
    if (ssid.length() < sizeof(config_.ssid)) {
        strncpy(config_.ssid, ssid.c_str(), sizeof(config_.ssid) - 1);
        config_.ssid[sizeof(config_.ssid) - 1] = '\0';
    }
    return config_.ssid;
}

const char* WiFiManager::get_ip() const {
    if (!is_connected()) {
        return nullptr;
    }
    
    updateIpCache();
    return ip_address_;
}

esp_err_t WiFiManager::load_credentials() {
    std::string ssid_str;
    std::string password_str;
    
    // Carregar SSID usando Storage
    ErrorCode err = Storage::loadConfig(CONFIG_KEY_SSID, ssid_str);
    if (err != CommonErrorCodes::None) {
        if (err == CommonErrorCodes::FileNotFound || err == CommonErrorCodes::FileIsEmpty) {
            return ESP_ERR_NOT_FOUND;
        }
        ESP_LOGE(TAG, "Erro ao carregar SSID: %s", err.description().c_str());
        return ESP_FAIL;
    }
    
    // Copiar SSID para config_
    if (ssid_str.length() < sizeof(config_.ssid)) {
        strncpy(config_.ssid, ssid_str.c_str(), sizeof(config_.ssid) - 1);
        config_.ssid[sizeof(config_.ssid) - 1] = '\0';
    } else {
        ESP_LOGE(TAG, "SSID muito longo");
        return ESP_ERR_INVALID_SIZE;
    }
    
    // Carregar senha (opcional - pode não existir para WiFi aberto)
    err = Storage::loadConfig(CONFIG_KEY_PASSWORD, password_str);
    if (err == CommonErrorCodes::FileNotFound || err == CommonErrorCodes::FileIsEmpty) {
        // Senha não configurada (WiFi aberto)
        config_.password[0] = '\0';
        return ESP_OK;
    } else if (err != CommonErrorCodes::None) {
        ESP_LOGE(TAG, "Erro ao carregar senha: %s", err.description().c_str());
        return ESP_FAIL;
    }
    
    // Copiar senha para config_
    if (password_str.length() < sizeof(config_.password)) {
        strncpy(config_.password, password_str.c_str(), sizeof(config_.password) - 1);
        config_.password[sizeof(config_.password) - 1] = '\0';
    } else {
        ESP_LOGE(TAG, "Senha muito longa");
        return ESP_ERR_INVALID_SIZE;
    }
    
    return ESP_OK;
}

esp_err_t WiFiManager::save_credentials() {
    std::string ssid_str(config_.ssid);
    std::string password_str(config_.password);
    
    // Salvar SSID usando Storage
    ErrorCode err = Storage::storeConfig(CONFIG_KEY_SSID, ssid_str, true);
    if (err != CommonErrorCodes::None) {
        ESP_LOGE(TAG, "Erro ao salvar SSID: %s", err.description().c_str());
        return ESP_FAIL;
    }
    
    // Salvar senha (mesmo que vazia para WiFi aberto)
    err = Storage::storeConfig(CONFIG_KEY_PASSWORD, password_str, true);
    if (err != CommonErrorCodes::None) {
        ESP_LOGE(TAG, "Erro ao salvar senha: %s", err.description().c_str());
        return ESP_FAIL;
    }
    
    return ESP_OK;
}

int WiFiManager::scan(wifi_ap_record_t* ap_list, uint16_t max_aps) {
    if (!wifi_connection_) {
        ESP_LOGE(TAG, "WiFi Connection não inicializado");
        return -1;
    }
    
    if (ap_list == nullptr || max_aps == 0) {
        ESP_LOGE(TAG, "Parâmetros inválidos para scan");
        return -1;
    }
    
    return wifi_connection_->scan(ap_list, max_aps);
}

void WiFiManager::updateIpCache() const {
    if (!wifi_connection_ || !is_connected()) {
        ip_address_[0] = '\0';
        return;
    }
    
    IPAddress ip = wifi_connection_->getIPAddress();
    std::string ip_str = ip.toString();
    strncpy(ip_address_, ip_str.c_str(), sizeof(ip_address_) - 1);
    ip_address_[sizeof(ip_address_) - 1] = '\0';
}
