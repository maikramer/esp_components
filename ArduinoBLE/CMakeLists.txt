set(srcs
        ../submodules/arduino-esp32/libraries/BLE/src/BLE2902.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLE2904.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEAddress.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEAdvertisedDevice.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEAdvertising.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLECharacteristicMap.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEClient.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEDescriptor.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEDescriptorMap.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEExceptions.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLERemoteCharacteristic.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLERemoteDescriptor.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLERemoteService.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEScan.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLESecurity.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEService.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEServiceMap.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEUtils.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEUUID.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEValue.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/GeneralUtils.cpp
        ../submodules/arduino-esp32/cores/esp32/esp32-hal-log.h
        ../submodules/arduino-esp32/libraries/BLE/src/FreeRTOS.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLECharacteristic.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEServer.cpp
        ../submodules/arduino-esp32/libraries/BLE/src/BLEDevice.cpp
        )

set(requires bt nvs_flash utility esp_ringbuf)
set(include_dirs . ../submodules/arduino-esp32/libraries/BLE/src/ ../submodules/arduino-esp32/cores/esp32/)


idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c++17 -Wno-format-truncation)
target_compile_definitions(${COMPONENT_LIB} PRIVATE ringbuf_type_t=RingbufferType_t)
