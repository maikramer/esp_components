//
// Created by maikeu on 02/09/22.
//

#ifndef PROJETO_BETA_SIMPLEUSERMANAGER_H
#define PROJETO_BETA_SIMPLEUSERMANAGER_H


#include "UserManager.h"
#include "SimpleUser.h"
#include "projectConfig.h"

#ifdef USER_MANAGEMENT_ENABLED

class SimpleUserManager : public UserManager {
    ConnectedUser *CreateUserInstance() override {
        ESP_LOGI(__FUNCTION__, "Criando um SimpleUser");
        return new SimpleUser();
    }
};

#endif
#endif //PROJETO_BETA_SIMPLEUSERMANAGER_H
