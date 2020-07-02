//
// Created by maikeu on 18/08/2019.
//

#include <cstdio>
#include <esp_log.h>
#include <sstream>
#include <Utility.h>
#include <priorities.h>
#include "Commander.h"
#include "UserManager.h"
#include "vector"

typedef struct {
    DeviceCommandFunctionPtr Function{};
    std::vector<std::string> Data;
    BluetoothConnection *Connection{};
} Command_t;

static QueueHandle_t xCommandQueue = xQueueCreate(5, sizeof(void *));//NOLINT

static void CommandExecuterTask(void *arg __unused) {
    for (;;) {
        Command_t *command = nullptr;
        if (xQueueReceive(xCommandQueue, &command, portMAX_DELAY) == pdPASS) {
            command->Function(command->Data, command->Connection);
            delete command;
        }
    }
}

Commander::Commander() {
    Utility::CreateAndProfile("CommandExecuterTask", CommandExecuterTask, 4096, HIGH_PRIORITY, 0, nullptr);



}

void Commander::AddCommand(DeviceCommand command){
    _commands.push_back(command);
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
            auto *commandToSend = new Command_t;
            commandToSend->Function = command.Function;
            commandToSend->Data = data;
            commandToSend->Connection = connection;
            xQueueSendToBack(xCommandQueue, &commandToSend, portMAX_DELAY);
        }
    }
}