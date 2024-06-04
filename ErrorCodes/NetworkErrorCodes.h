#ifndef NETWORK_ERROR_CODES_H
#define NETWORK_ERROR_CODES_H

#include "ErrorCode.h"

/**
 * @file NetworkErrorCodes.h
 * @brief Defines error codes related to network operations.
 */

namespace CommonErrorCodes {

    // Network Errors
    extern const ErrorCode NetworkDown;        /**< The network connection is down or unavailable. */
    extern const ErrorCode HostUnreachable;     /**< The specified host is unreachable. */
    extern const ErrorCode ConnectionRefused;    /**< The connection attempt was refused. */
    extern const ErrorCode AddressInUse;         /**< The requested network address is already in use. */

}

#endif // NETWORK_ERROR_CODES_H