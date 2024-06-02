#ifndef WIFI_CONNECTION_H
#define WIFI_CONNECTION_H

#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "Event.h"
#include "IPAddress.h"
#include "CommonErrorCodes.h"

/**
 * @file WifiConnection.h
 * @brief This file defines the WifiConnection class for managing WiFi connections in station mode.
 */

/**
 * @class WifiConnection
 * @brief Manages WiFi connection to an access point in station (STA) mode.
 */
class WifiConnection {
public:
    /**
     * @brief Constructor.
     */
    WifiConnection();

    /**
     * @brief Destructor. Disconnects from the WiFi network if connected.
     */
    ~WifiConnection();

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
    static void disconnect() ;

    /**
     * @brief Checks if the device is currently connected to a WiFi network.
     *
     * @return True if connected, false otherwise.
     */
    [[nodiscard]] static bool isConnected() ;

    /**
     * @brief Gets the SSID of the currently connected WiFi network.
     *
     * @return The SSID as a string, or an empty string if not connected.
     */
    [[nodiscard]] static std::string getSSID() ;

    /**
     * @brief Gets the IP address of the device on the WiFi network.
     *
     * @return The IPAddress, or an invalid IPAddress if not connected.
     */
    [[nodiscard]] IPAddress getIPAddress() const;

    /**
     * @brief Event triggered when a connection to a WiFi network is established.
     */
    Event<> onConnected;

    /**
     * @brief Event triggered when the WiFi connection is lost or fails.
     */
    Event<> onDisconnected;

private:
    static void eventHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    static EventGroupHandle_t wifiEventGroup; /**< Event group for WiFi events. */
    static const int WIFI_CONNECTED_BIT = BIT0; /**< Event bit for successful connection. */
    static const int WIFI_FAIL_BIT = BIT1; /**< Event bit for connection failure. */
    static const char* TAG; /**< Log tag for the class. */

    std::string _ssid; /**< SSID of the target WiFi network. */
    std::string _password; /**< Password of the target WiFi network. */
    IPAddress _ipAddress; /**< The current IP address of the device. */
    int _retryNum; /**< Number of connection retry attempts. */
};

#endif // WIFI_CONNECTION_H