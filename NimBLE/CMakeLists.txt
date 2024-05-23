set(srcs
        ../submodules/esp-nimble-cpp/src/NimBLE2904.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEAddress.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEAdvertisedDevice.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEAdvertising.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEClient.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEDescriptor.cpp
        ../submodules/esp-nimble-cpp/src/NimBLERemoteCharacteristic.cpp
        ../submodules/esp-nimble-cpp/src/NimBLERemoteDescriptor.cpp
        ../submodules/esp-nimble-cpp/src/NimBLERemoteService.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEScan.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEService.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEUtils.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEUUID.cpp
        ../submodules/esp-nimble-cpp/src/NimBLECharacteristic.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEServer.cpp
        ../submodules/esp-nimble-cpp/src/NimBLEDevice.cpp
        )

set(requires bt nvs_flash utility esp_ringbuf)
set(include_dirs . ../submodules/esp-nimble-cpp/src/)


idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_NIMBLE_CPP_IDF)
