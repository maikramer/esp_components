#include "Wifi.h"
#include <iostream>

/**
 * @brief Constructor.
 */
Wifi::Wifi() : WifiConnection(), WifiTelnet(), WifiOta() {
    // Constructor implementation
}

/**
 * @brief Initializes WiFi connection.
 * @param ssid SSID of the WiFi network.
 * @param password Password of the WiFi network.
 */
void Wifi::begin(const char* ssid, const char* password) {
    WifiConnection::connect(ssid, password);
    WifiTelnet::init();
}

/**
 * @brief Handles incoming client connections for Telnet.
 */
void Wifi::handleClient() {
    WifiTelnet::handleClient();
}

/**
 * @brief Prints the connection status.
 */
void Wifi::printStatus() {
    if (WifiConnection::isConnected()) {
        std::cout << "Connected to WiFi: " << WifiConnection::getSSID() << std::endl;
        std::cout << "IP Address: " << WifiConnection::getIPAddress() << std::endl;
    } else {
        std::cout << "Not connected to WiFi" << std::endl;
    }
}
