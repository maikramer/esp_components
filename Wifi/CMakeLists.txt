set(srcs Wifi.cpp Telnet.cpp IPAddress.cpp WirelessDevice.cpp WifiConnection.cpp WifiOta.cpp WifiTelnet.cpp WifiClient.cpp WifiServer.cpp)
set(requires esp_wifi nvs_flash utility esp_https_ota Connection ErrorCodes)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")