#include "CommonErrorCodes.h"

/**
 * @file CommonErrorCodes.cpp
 * @brief Implementation of the common error codes.
 */

namespace CommonErrorCodes {

    // General Errors
    const ErrorCode Invalid = ErrorCode::define("Invalid", "Invalid Error Code", ErrorCodeType::General);
    const ErrorCode None = ErrorCode::define("None", "No error occurred", ErrorCodeType::General);
    const ErrorCode UnknownError = ErrorCode::define("UnknownError", "An unexpected or unknown error occurred", ErrorCodeType::General);
    const ErrorCode OperationFailed = ErrorCode::define("OperationFailed", "The requested operation failed", ErrorCodeType::General);
    const ErrorCode NotImplemented = ErrorCode::define("NotImplemented", "Functionality not implemented yet", ErrorCodeType::General);
    const ErrorCode ArgumentError = ErrorCode::define("ArgumentError", "Invalid argument provided", ErrorCodeType::General);

    // Network Errors
    const ErrorCode NetworkDown = ErrorCode::define("NetworkDown", "The network connection is down or unavailable", ErrorCodeType::Network);
    const ErrorCode ConnectionTimeout = ErrorCode::define("ConnectionTimeout", "The network connection timed out", ErrorCodeType::Network);
    const ErrorCode HostUnreachable = ErrorCode::define("HostUnreachable", "The specified host is unreachable", ErrorCodeType::Network);
    const ErrorCode ConnectionRefused = ErrorCode::define("ConnectionRefused", "The connection attempt was refused", ErrorCodeType::Network);

    // WiFi Errors
    const ErrorCode WifiInitFailed = ErrorCode::define("WifiInitFailed", "WiFi initialization failed", ErrorCodeType::WiFi);
    const ErrorCode WifiConnectionFailed = ErrorCode::define("WifiConnectionFailed", "Failed to connect to WiFi network", ErrorCodeType::WiFi);
    const ErrorCode WifiAPStartFailed = ErrorCode::define("WifiAPStartFailed", "Failed to start WiFi Access Point", ErrorCodeType::WiFi);
    const ErrorCode WifiScanFailed = ErrorCode::define("WifiScanFailed", "WiFi scan for networks failed", ErrorCodeType::WiFi);
    const ErrorCode WifiNetworkNotFound = ErrorCode::define("WifiNetworkNotFound", "The specified WiFi network was not found", ErrorCodeType::WiFi);

    // Socket Errors
    const ErrorCode SocketCreationFailed = ErrorCode::define("SocketCreationFailed", "Failed to create a socket", ErrorCodeType::Socket);
    const ErrorCode SocketBindFailed = ErrorCode::define("SocketBindFailed", "Failed to bind the socket", ErrorCodeType::Socket);
    const ErrorCode SocketListenFailed = ErrorCode::define("SocketListenFailed", "Failed to listen on the socket", ErrorCodeType::Socket);
    const ErrorCode SocketAcceptFailed = ErrorCode::define("SocketAcceptFailed", "Failed to accept a client connection", ErrorCodeType::Socket);
    const ErrorCode SocketConnectFailed = ErrorCode::define("SocketConnectFailed", "Failed to connect to remote host", ErrorCodeType::Socket);
    const ErrorCode SocketSendFailed = ErrorCode::define("SocketSendFailed", "Failed to send data on the socket", ErrorCodeType::Socket);
    const ErrorCode SocketReceiveFailed = ErrorCode::define("SocketReceiveFailed", "Failed to receive data on the socket", ErrorCodeType::Socket);
    const ErrorCode SocketClosed = ErrorCode::define("SocketClosed", "The socket has been closed", ErrorCodeType::Socket);

    // File Errors
    const ErrorCode FileNotFound = ErrorCode::define("FileNotFound", "The specified file was not found", ErrorCodeType::File);
    const ErrorCode FileOpenError = ErrorCode::define("FileOpenError", "Error opening the file", ErrorCodeType::File);
    const ErrorCode FileReadError = ErrorCode::define("FileReadError", "Error reading from the file", ErrorCodeType::File);
    const ErrorCode FileWriteError = ErrorCode::define("FileWriteError", "Error writing to the file", ErrorCodeType::File);
    const ErrorCode FileExists = ErrorCode::define("FileExists", "A file with the specified name already exists", ErrorCodeType::File);

    // Memory Errors
    const ErrorCode MemoryAllocationFailed = ErrorCode::define("MemoryAllocationFailed", "Failed to allocate memory", ErrorCodeType::Memory);

    // Storage Errors 
    const ErrorCode StorageInitFailed = ErrorCode::define("StorageInitFailed", "Storage device initialization failed", ErrorCodeType::Storage);
    const ErrorCode StorageReadError = ErrorCode::define("StorageReadError", "Error reading data from storage device", ErrorCodeType::Storage);
    const ErrorCode StorageWriteError = ErrorCode::define("StorageWriteError", "Error writing data to storage device", ErrorCodeType::Storage);
    const ErrorCode StorageNotMounted = ErrorCode::define("StorageNotMounted", "Storage device is not mounted", ErrorCodeType::Storage);

    // User-Related Errors
    const ErrorCode AuthenticationFailed = ErrorCode::define("AuthenticationFailed", "User authentication failed", ErrorCodeType::User);
    const ErrorCode UserNotFound = ErrorCode::define("UserNotFound", "The specified user was not found", ErrorCodeType::User);

    // Communication Errors
    const ErrorCode CommunicationError = ErrorCode::define("CommunicationError", "A communication error occurred", ErrorCodeType::Communication);
    const ErrorCode CommunicationTimeout = ErrorCode::define("CommunicationTimeout", "Communication timeout", ErrorCodeType::Communication);
    const ErrorCode ChecksumError = ErrorCode::define("ChecksumError", "Checksum or CRC verification failed", ErrorCodeType::Communication);

    // Hardware Errors
    const ErrorCode SensorError = ErrorCode::define("SensorError", "Sensor reading failed or is invalid", ErrorCodeType::Hardware);
    const ErrorCode DeviceNotResponding = ErrorCode::define("DeviceNotResponding", "Hardware device not responding", ErrorCodeType::Hardware);
}