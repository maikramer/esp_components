set(srcs Telnet.cpp IPAddress.cpp WirelessDevice.cpp WifiConnection.cpp WifiOta.cpp WifiTelnet.cpp WifiClient.cpp WifiServer.cpp WiFiManager.cpp OtaManager.cpp)
set(include_dirs .)
set(requires esp_wifi nvs_flash esp_netif esp_event Utility esp_https_ota esp_http_client efuse Connection ErrorCodes Storage)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")