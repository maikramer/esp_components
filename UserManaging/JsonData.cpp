//
// Created by maikeu on 30/09/2019.
//
#include "projectConfig.h"
#ifdef USER_MANAGEMENT_ENABLED

#include "JsonData.h"

namespace JsonData {

    void to_json(nlohmann::json &j, const User &user) {
        j = nlohmann::json{{"Name",        user.Name},
                           {"Password",    user.Password},
                           {"Email",       user.Email},
                           {"IsConfirmed", user.IsConfirmed}};
    }

    void from_json(const nlohmann::json &j, User &user) {
        j.at("Name").get_to(user.Name);
        j.at("Password").get_to(user.Password);
        j.at("Email").get_to(user.Email);
        j.at("IsConfirmed").get_to(user.IsConfirmed);
    }
}

#endif