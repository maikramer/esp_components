#include "NetworkErrorCodes.h"

/**
 * @file NetworkErrorCodes.cpp
 * @brief Implementation of network-related error codes.
 */

namespace CommonErrorCodes {
    const ErrorCode NetworkDown = ErrorCode::define("NetworkDown", "Network connection is down", ErrorCodeType::Network);
    const ErrorCode HostUnreachable = ErrorCode::define("HostUnreachable", "Host is unreachable", ErrorCodeType::Network);
    const ErrorCode ConnectionRefused = ErrorCode::define("ConnectionRefused", "Connection refused", ErrorCodeType::Network);
    const ErrorCode AddressInUse = ErrorCode::define("AddressInUse", "Network address is in use", ErrorCodeType::Network);
}