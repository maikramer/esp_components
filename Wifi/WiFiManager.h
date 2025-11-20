#pragma once

#include "esp_err.h"
#include "esp_wifi.h"  // Para wifi_ap_record_t
#include "WifiConnection.h"
#include "Storage.h"
#include <cstdint>
#include <cstring>
#include <string>

/**
 * @file WiFiManager.h
 * @brief Singleton wrapper para WifiConnection com persistência de credenciais no NVS
 */

struct WiFiConfig {
    char ssid[33];      // SSID máximo 32 caracteres + null terminator
    char password[65];  // Senha WiFi máximo 64 caracteres + null terminator
};

/**
 * @class WiFiManager
 * @brief Singleton wrapper para WifiConnection que mantém credenciais no NVS
 */
class WiFiManager {
public:
    static WiFiManager& instance();
    
    // Inicializar WiFi Manager (carrega credenciais e conecta automaticamente se houver)
    esp_err_t init();
    
    // Configurar credenciais WiFi e conectar
    esp_err_t connect(const char* ssid, const char* password);
    
    // Desconectar WiFi
    esp_err_t disconnect();
    
    // Verificar se está conectado
    bool is_connected() const;
    
    // Obter SSID atual
    const char* get_ssid() const;
    
    // Obter IP atual (retorna nullptr se não conectado)
    const char* get_ip() const;
    
    // Carregar credenciais do NVS
    esp_err_t load_credentials();
    
    // Salvar credenciais no NVS
    esp_err_t save_credentials();
    
    // Obter configuração atual (read-only)
    const WiFiConfig& config() const { return config_; }
    
    // Escanear redes WiFi disponíveis
    // Retorna número de redes encontradas, ou negativo em caso de erro
    int scan(wifi_ap_record_t* ap_list, uint16_t max_aps);
    
    // Obter referência ao WifiConnection subjacente (para uso avançado)
    WifiConnection& getConnection() { return *wifi_connection_; }
    const WifiConnection& getConnection() const { return *wifi_connection_; }

private:
    WiFiManager();
    ~WiFiManager();
    WiFiManager(const WiFiManager&) = delete;
    WiFiManager& operator=(const WiFiManager&) = delete;
    
    WifiConnection* wifi_connection_;  // Instância da biblioteca Wifi
    bool initialized_;
    mutable WiFiConfig config_;  // mutable para permitir cache em funções const
    mutable char ip_address_[16] = {}; // IPv4: "xxx.xxx.xxx.xxx\0" (cache)
    
    static constexpr const char* CONFIG_KEY_SSID = "wifi_ssid";
    static constexpr const char* CONFIG_KEY_PASSWORD = "wifi_password";
    
    // Helper para atualizar cache de IP
    void updateIpCache() const;
};
