#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "BaseConnection.h"
#include "IPAddress.h"
#include "WifiErrorCodes.h"
#include "WifiClient.h"

/**
 * @file WifiConnection.h
 * @brief This file defines the WifiConnection class for managing WiFi connections in station mode.
 */

/**
 * @class WifiConnection
 * @brief Manages a WiFi connection in station (STA) mode, inheriting from BaseConnection.
 */
class WifiConnection : public BaseConnection {
public:
    /**
     * @brief Constructor.
     */
    WifiConnection();

    /**
     * @brief Destructor. Disconnects from the WiFi network if connected.
     */
    ~WifiConnection() override;

    /**
     * @brief Connects to a WiFi access point.
     *
     * @param ssid The SSID (name) of the WiFi network.
     * @param password The password for the WiFi network.
     * @return ErrorCode indicating success or failure.
     */
    ErrorCode connect(const std::string& ssid, const std::string& password);

    /**
     * @brief Disconnects from the currently connected WiFi network.
     */
    void disconnect() override;

    /**
     * @brief Checks if the device is currently connected to a WiFi network.
     *
     * @return True if connected, false otherwise.
     */
    [[nodiscard]] bool isConnected() const override;

    /**
     * @brief Gets the SSID of the currently connected WiFi network.
     *
     * @return The SSID as a string, or an empty string if not connected.
     */
    [[nodiscard]] std::string getSSID() const;

    /**
     * @brief Gets the IP address of the device on the WiFi network.
     *
     * @return The IPAddress, or an invalid IPAddress if not connected.
     */
    [[nodiscard]] IPAddress getIPAddress() const;

    /**
     * @brief Sends raw byte data over the WiFi connection.
     *
     * @param data The data buffer to send.
     * @param length The length of the data buffer.
     * @return ErrorCode indicating success or failure.
     */
    ErrorCode sendRawData(const uint8_t* data, size_t length) const override;

    /**
     * @brief Gets the connection ID associated with this WifiConnection.
     *
     * @return The connection ID.
     */
    [[nodiscard]] uint16_t getId() const;

    void setWifiClient(WifiClient* wifiClient) {
        _wifiClient = wifiClient;
    }

private:
    static void eventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static EventGroupHandle_t _wifiEventGroup; /**< Event group for WiFi events. */
    static const int WIFI_CONNECTED_BIT = BIT0; /**< Event bit for successful connection. */
    static const int WIFI_FAIL_BIT = BIT1; /**< Event bit for connection failure. */
    static const char* TAG; /**< Log tag for the class. */

    std::string _ssid; /**< SSID of the target WiFi network. */
    std::string _password; /**< Password of the target WiFi network. */
    IPAddress _ipAddress; /**< The current IP address of the device. */
    int _retryNum; /**< Number of connection retry attempts. */
    bool _isConnected; /**< Flag indicating if the connection is active. */
    uint16_t _connId; /**< Connection ID for this WiFi connection (currently not used, but can be implemented in the future). */
    WifiClient* _wifiClient; /**< Pointer to the WifiClient object to use for data transmission. */
};

#endif // WIFI_CONNECTION_H