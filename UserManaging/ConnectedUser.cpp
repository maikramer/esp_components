//
// Created by maikeu on 09/06/2020.
//

#include "ConnectedUser.h"
void ConnectedUser::Clear() {
    User = "";
    IsLogged = false;
    IsAdmin = false;
    IsFirstAdminLogin = false;
    IsContinuingToUse = false;
    OnClear();
}