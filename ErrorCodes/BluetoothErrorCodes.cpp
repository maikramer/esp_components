#include "BluetoothErrorCodes.h"

/**
 * @file BluetoothErrorCodes.cpp
 * @brief Implementation of Bluetooth-related error codes.
 */

namespace CommonErrorCodes {
    const ErrorCode BluetoothInitFailed = ErrorCode::define("BluetoothInitFailed", "Bluetooth initialization failed", ErrorCodeType::Bluetooth);
    const ErrorCode BluetoothConnectionFailed = ErrorCode::define("BluetoothConnectionFailed", "Failed to establish Bluetooth connection", ErrorCodeType::Bluetooth);
    const ErrorCode BluetoothServiceCreationFailed = ErrorCode::define("BluetoothServiceCreationFailed", "Failed to create Bluetooth service", ErrorCodeType::Bluetooth);
    const ErrorCode BluetoothCharacteristicCreationFailed = ErrorCode::define("BluetoothCharacteristicCreationFailed", "Failed to create Bluetooth characteristic", ErrorCodeType::Bluetooth);
    // ... Add definitions for more Bluetooth error codes ...
}