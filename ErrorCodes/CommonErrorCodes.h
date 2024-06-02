#ifndef COMMON_ERROR_CODES_H
#define COMMON_ERROR_CODES_H

#include "ErrorCode.h"

/**
 * @file CommonErrorCodes.h
 * @brief Defines a set of common error codes that can be used across various modules.
 */

namespace CommonErrorCodes {

    // General Errors
    extern const ErrorCode Invalid;              /**< Invalid error code. */
    extern const ErrorCode None;                 /**< No error occurred. */
    extern const ErrorCode UnknownError;          /**< An unexpected or unknown error occurred. */
    extern const ErrorCode OperationFailed;       /**< The requested operation failed. */
    extern const ErrorCode NotImplemented;      /**< Functionality not yet implemented. */
    extern const ErrorCode ArgumentError;       /**< Invalid argument provided to a function or method. */

    // Network Errors
    extern const ErrorCode NetworkDown;           /**< The network connection is down or unavailable. */
    extern const ErrorCode ConnectionTimeout;      /**< The network connection timed out. */
    extern const ErrorCode HostUnreachable;        /**< The specified host is unreachable. */
    extern const ErrorCode ConnectionRefused;     /**< The connection attempt was refused. */

    // WiFi Errors
    extern const ErrorCode WifiInitFailed;         /**< Initialization of the WiFi module failed. */
    extern const ErrorCode WifiConnectionFailed;    /**< Failed to connect to the specified WiFi network. */
    extern const ErrorCode WifiAPStartFailed;       /**< Failed to start the WiFi Access Point. */
    extern const ErrorCode WifiScanFailed;          /**< WiFi scan for networks failed. */
    extern const ErrorCode WifiNetworkNotFound;     /**< The specified WiFi network was not found. */

    // Socket Errors
    extern const ErrorCode SocketCreationFailed;     /**< Failed to create a new socket. */
    extern const ErrorCode SocketBindFailed;        /**< Failed to bind the socket to an address and port. */
    extern const ErrorCode SocketListenFailed;       /**< Failed to put the socket into listening mode. */
    extern const ErrorCode SocketAcceptFailed;       /**< Failed to accept a new client connection on the socket. */
    extern const ErrorCode SocketConnectFailed;     /**< Failed to connect to a remote host using the socket. */
    extern const ErrorCode SocketSendFailed;         /**< Failed to send data on the socket. */
    extern const ErrorCode SocketReceiveFailed;      /**< Failed to receive data on the socket. */
    extern const ErrorCode SocketClosed;             /**< The socket has been closed. */

    // File Errors
    extern const ErrorCode FileNotFound;            /**< The specified file was not found. */
    extern const ErrorCode FileOpenError;           /**< Error opening the file (e.g., permissions issue). */
    extern const ErrorCode FileReadError;          /**< Error reading data from the file. */
    extern const ErrorCode FileWriteError;         /**< Error writing data to the file. */
    extern const ErrorCode FileExists;              /**< A file with the specified name already exists. */

    // Memory Errors
    extern const ErrorCode MemoryAllocationFailed;  /**< Failed to allocate the requested memory. */

    // Storage Errors (SD Card, Flash, etc.)
    extern const ErrorCode StorageInitFailed;       /**< Storage device initialization failed. */
    extern const ErrorCode StorageReadError;        /**< Error reading data from the storage device. */
    extern const ErrorCode StorageWriteError;       /**< Error writing data to the storage device. */
    extern const ErrorCode StorageNotMounted;      /**< The storage device is not mounted or accessible. */

    // User-Related Errors
    extern const ErrorCode AuthenticationFailed;   /**< User authentication failed (e.g., incorrect password). */
    extern const ErrorCode UserNotFound;           /**< The specified user was not found. */

    // Communication Errors (Serial, I2C, SPI, etc.)
    extern const ErrorCode CommunicationError;      /**< A general communication error occurred. */
    extern const ErrorCode CommunicationTimeout;     /**< A timeout occurred during communication. */
    extern const ErrorCode ChecksumError;           /**< Data checksum or CRC verification failed. */

    // Hardware Errors
    extern const ErrorCode SensorError;             /**< A sensor reading failed or returned an invalid value. */
    extern const ErrorCode DeviceNotResponding;    /**< A hardware device is not responding. */

    // ... Add more common error codes for other types as needed ...
}

#endif // COMMON_ERROR_CODES_H