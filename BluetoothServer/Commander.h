//
// Created by maikeu on 18/08/2019.
//

#ifndef TOMADA_SMART_CONDO_COMMANDER_H
#define TOMADA_SMART_CONDO_COMMANDER_H

#include <string>
#include <utility>
#include <list>
#include <vector>
#include <nameof.hpp>
#include "BluetoothConnection.h"

class BluetoothConnection;

//enum class CommandCode : uint8_t {
//    StartCode = 0,
//    FinalizeCode = 1,
//    GetConsumeHistoryCode = 2,
//    GetTotalConsumeInPeriodCode = 3,
//    GetUsersTotalConsumeMapCode = 4,
//    SetAdminInfoCode = 5,
//    LoginCode = 6,
//    LogoffCode = 7,
//    SignUpCode = 8,
//    GetUsersWaitingCode = 9,
//    ApproveUserCode = 10,
//    UserCommand = 11
//};

class DeviceCommand {
public:
    DeviceCommand(const uint32_t dataSize, std::string internalName, const uint8_t code,
                  std::function<void(const std::vector<std::string> &, BluetoothConnection *)> functionPtr) : DataSize(
            dataSize),
                                                                                                              InternalName(
                                                                                                                      std::move(
                                                                                                                              internalName)),
                                                                                                              Code(code),
                                                                                                              Function(
                                                                                                                      std::move(
                                                                                                                              functionPtr)) {}

    const uint32_t DataSize;
    std::string InternalName;
    const uint8_t Code;
    std::function<void(const std::vector<std::string> &, BluetoothConnection *)> Function;
};

class Commander {
public:
    Commander() = delete;

    static void CheckForCommand(const std::string &rxValue, BluetoothConnection *connection);

    static void AddCommand(const DeviceCommand &command);

    static void Init();

private:
    static std::list<DeviceCommand> _commands;

};


#endif //TOMADA_SMART_CONDO_COMMANDER_H
