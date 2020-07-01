//
// Created by maikeu on 25/09/2019.
//

#ifndef TOMADA_SMART_CONDO_CONNECTEDUSER_H
#define TOMADA_SMART_CONDO_CONNECTEDUSER_H

#include <cstdint>
#include <string>
#include <Storage.h>
#include <SdCard.h>
#include "Enums.h"

class EnergyPlug;

class ConnectedUser {
public:
    std::string User;
    bool IsLogged = false;
    bool IsAdmin = false;
    bool IsFirstAdminLogin = false;
    bool IsContinuingToUse = false;
    EnergyPlug *Plug = nullptr;

    auto operator==(const ConnectedUser &other) -> bool {
        return this == &other;
    }

    void Clear();

};

#endif //TOMADA_SMART_CONDO_CONNECTEDUSER_H
