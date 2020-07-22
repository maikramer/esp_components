//
// Created by maikeu on 09/06/2020.
//
#include "projectConfig.h"

#ifdef USER_MANAGEMENT_ENABLED

#include "ConnectedUser.h"

void ConnectedUser::Clear() {
    User = "";
    IsLogged = false;
    IsAdmin = false;
    IsContinuingToUse = false;
    OnClear();
}

#endif