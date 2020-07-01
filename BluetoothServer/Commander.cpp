//
// Created by maikeu on 18/08/2019.
//

#include <cstdio>
#include <esp_log.h>
#include <sstream>
#include <Utility.h>
#include <priorities.h>
#include "Commander.h"
#include "EnergyPlugManager.h"
#include "UserManager.h"
#include "UserCommander.h"
#include "vector"

typedef struct {
    DeviceCommandFunctionPtr Function;
    std::vector<std::string> Data;
    BluetoothConnection *Connection;
} Command_t;

static QueueHandle_t xCommandQueue = xQueueCreate(5, sizeof(void *));

static void CommandExecuterTask(void *arg __unused) {
    for (;;) {
        Command_t *command;
        if (xQueueReceive(xCommandQueue, &command, portMAX_DELAY) == pdPASS) {
            command->Function(command->Data, command->Connection);
            delete command;
        }
    }
}

Commander::Commander() {
    Utility::CreateAndProfile("CommandExecuterTask", CommandExecuterTask, 4096, HIGH_PRIORITY, 0, nullptr);
    const DeviceCommand Start(0, "Start", CommandCode::StartCode,
                              [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                  EnergyPlugManager::StartUse(connection);
                              });

    const DeviceCommand Finalize(1, "Finalize", CommandCode::FinalizeCode,
                                 [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                     EnergyPlugManager::Finalize(data[0], connection);
                                 });

    const DeviceCommand GetUsage(0, "GetUsage", CommandCode::GetUsageCode,
                                 [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                     EnergyPlugManager::SendUsage(connection);
                                 });

    const DeviceCommand GetAccum(0, "GetAccum", CommandCode::GetAccumCode,
                                 [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                     EnergyPlugManager::SendAccumulated(connection);
                                 });

    const DeviceCommand SetAdminInfo(2, "SetAdminInfo", CommandCode::SetAdminInfoCode,
                                     [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                         UserManager::SetAdmin(data, connection);
                                     });

    const DeviceCommand Login(2, "Login", CommandCode::LoginCode,
                              [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                  UserManager::Login(data, connection);
                              });

    const DeviceCommand Logoff(0, "Logoff", CommandCode::LogoffCode,
                               [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                   UserManager::Logoff(connection);
                               });

    const DeviceCommand SignUp(1, "SignUp", CommandCode::SignUpCode,
                               [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                   UserManager::SignUp(data[0], connection);
                               });

    const DeviceCommand GetUsersWaiting(0, "GetUsersWaiting", CommandCode::GetUsersWaitingCode,
                                        [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                            UserManager::GetUsersWaitingForApproval(connection);
                                        });

    const DeviceCommand ApproveUser(1, "ApproveUser", CommandCode::ApproveUserCode,
                                    [](const std::vector<std::string> &data,
                                       BluetoothConnection *connection) {
                                        UserManager::ApproveUser(data[0]);
                                    });

    const DeviceCommand GetUsersAccumulatedUsageMap(0, "GetUsersWaiting", CommandCode::GetUsersAccumulatedUsageMap,
                                                    [](const std::vector<std::string> &data,
                                                       BluetoothConnection *connection) {
                                                        UserManager::GetUsersAccumulated(data, connection);
                                                    });

    const DeviceCommand UserCommand(0, "UserCommand", CommandCode::UserCommand,
                                    [](const std::vector<std::string> &data, BluetoothConnection *connection) {
                                        UserCommander::ExecuteCommand(data[0], connection);
                                    });

    _commands.push_back(Start);
    _commands.push_back(Finalize);
    _commands.push_back(GetUsage);
    _commands.push_back(GetAccum);
    _commands.push_back(SetAdminInfo);
    _commands.push_back(Login);
    _commands.push_back(Logoff);
    _commands.push_back(SignUp);
    _commands.push_back(GetUsersWaiting);
    _commands.push_back(ApproveUser);
    _commands.push_back(GetUsersAccumulatedUsageMap);
    _commands.push_back(UserCommand);
}

std::list<DeviceCommand> Commander::_commands;//NOLINT

void Commander::CheckForCommand(const std::string &rxValue, BluetoothConnection *connection) {
    //Extrai comando
    uint8_t commandCode = rxValue.c_str()[0];
    auto rxData = std::string(rxValue.c_str() + 1);
    auto data = Utility::split(rxData, ':');

    ESP_LOGI(__FUNCTION__, "Command: %u", commandCode);
    std::stringstream printData;
    for (const auto &d : data) {
        printData << d << " | ";
    }
    ESP_LOGI(__FUNCTION__, "Data : %s", printData.str().c_str());

    for (auto command : _commands) {
        if ((uint8_t) command.Code == commandCode) {
            ESP_LOGI(__FUNCTION__, "Comando encontrado: %s", command.InternalName);
            Command_t *commandToSend = new Command_t;
            commandToSend->Function = command.Function;
            commandToSend->Data = data;
            commandToSend->Connection = connection;
            xQueueSendToBack(xCommandQueue, &commandToSend, portMAX_DELAY);
        }
    }
}