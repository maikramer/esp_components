set(srcs Wifi.cpp Telnet.cpp IPAddress.cpp WirelessDevice.cpp WifiConnection.cpp WifiOta.cpp WifiTelnet.cpp WifiClient.cpp WifiServer.cpp)
set(include_dirs .)
set(requires esp_wifi nvs_flash esp_netif esp_event utility esp_https_ota Connection ErrorCodes)

idf_component_register(SRCS "WifiOta.cpp" "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")ires}")