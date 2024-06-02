#ifndef WIFI_H
#define WIFI_H

#include "WifiConnection.h"
#include "WifiTelnet.h"
#include "WifiOta.h"

/**
 * @class Wifi
 * @brief Manages WiFi connection, Telnet server, and OTA updates.
 */
class Wifi : public WifiConnection, public WifiTelnet, public WifiOta {
public:
    /**
     * @brief Constructor.
     */
    Wifi();

    /**
     * @brief Initializes WiFi connection.
     * @param ssid SSID of the WiFi network.
     * @param password Password of the WiFi network.
     */
    void begin(const char* ssid, const char* password);

    /**
     * @brief Handles incoming client connections for Telnet.
     */
    void handleClient() override;

    /**
     * @brief Prints the connection status.
     */
    void printStatus();
};

#endif // WIFI_H
