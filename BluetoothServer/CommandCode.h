//
// Created by maikeu on 18/07/2020.
//

#ifndef COMMANDCODE_H
#define COMMANDCODE_H

enum class CommandCode : uint8_t {
    SetAdminInfoCode = 0,
    LoginCode = 1,
    LogoffCode = 2,
    SignUpCode = 3,
    GetUsersWaitingCode = 4,
    ApproveUserCode = 5,
    UserCommand = 6

};

#endif //COMMANDCODE_H
