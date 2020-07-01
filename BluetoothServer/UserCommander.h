#include <vector>
#include <string>
#include <EnergyPlugManager.h>

#ifndef TOMADA_SMART_CONDO_USERCOMMANDER_H
#define TOMADA_SMART_CONDO_USERCOMMANDER_H

class UserCommander {
public:
    static void ExecuteCommand(const std::string &data, BluetoothConnection *connection) {
        if (data == "RecoverFromOverCurrent") {
            EnergyPlugManager::RecoverFromOverCurrent(connection->GetUser()->Plug);
        }
    }
};

#endif