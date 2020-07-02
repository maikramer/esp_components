//
// Created by maikeu on 18/08/2019.
//

#ifndef TOMADA_SMART_CONDO_COMMANDER_H
#define TOMADA_SMART_CONDO_COMMANDER_H

#include <string>
#include <utility>
#include <list>
#include <vector>
#include "BluetoothConnection.h"

class BluetoothConnection;

typedef void (*DeviceCommandFunctionPtr)(const std::vector<std::string> &, BluetoothConnection *);

enum class CommandCode : uint8_t {
    StartCode = 0,
    FinalizeCode = 1,
    GetUsageCode = 2,
    GetAccumCode = 3,
    GetUsersAccumulatedUsageMap = 4,
    SetAdminInfoCode = 5,
    LoginCode = 6,
    LogoffCode = 7,
    SignUpCode = 8,
    GetUsersWaitingCode = 9,
    ApproveUserCode = 10,
    UserCommand = 11
};

class DeviceCommand {
public:
    DeviceCommand(const uint32_t dataSize, const char *internalName, const CommandCode code,
                  DeviceCommandFunctionPtr functionPtr) : DataSize(dataSize),
                                                          InternalName(internalName),
                                                          Code(code),
                                                          Function(functionPtr) {}

    const uint32_t DataSize;
    const char *InternalName;
    const CommandCode Code;
    DeviceCommandFunctionPtr Function;
};

class Commander {
public:
    Commander();

    static void CheckForCommand(const std::string &rxValue, BluetoothConnection *connection);
    static void AddCommand(DeviceCommand command);

private:
    static std::list<DeviceCommand> _commands;
};


#endif //TOMADA_SMART_CONDO_COMMANDER_H
