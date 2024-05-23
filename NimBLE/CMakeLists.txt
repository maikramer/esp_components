set(srcs
        ../submodules/NimBLE-Arduino/src/NimBLE2904.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEAddress.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEAdvertisedDevice.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEAdvertising.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEClient.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEDescriptor.cpp
        ../submodules/NimBLE-Arduino/src/NimBLERemoteCharacteristic.cpp
        ../submodules/NimBLE-Arduino/src/NimBLERemoteDescriptor.cpp
        ../submodules/NimBLE-Arduino/src/NimBLERemoteService.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEScan.cpp
        ../submodules/NimBLE-Arduino/src/NimBLESecurity.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEService.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEUtils.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEUUID.cpp
        ../submodules/NimBLE-Arduino/src/NimBLECharacteristic.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEServer.cpp
        ../submodules/NimBLE-Arduino/src/NimBLEDevice.cpp
        )

set(requires bt nvs_flash utility esp_ringbuf)
set(include_dirs . ../submodules/NimBLE-Arduino/src/)


idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_definitions(${COMPONENT_LIB} PRIVATE CONFIG_NIMBLE_CPP_IDF)
