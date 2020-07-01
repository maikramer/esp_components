//
// Created by maikeu on 30/09/2019.
//

#ifndef TOMADA_SMART_CONDO_JSONDATA_H
#define TOMADA_SMART_CONDO_JSONDATA_H

#include <string>
#include <nlohmann/json.hpp>

namespace JsonData {
    struct User {
        using string = std::string;
        string Name;
        string Password;
        string Email;
        bool IsConfirmed = false;
    };

    void to_json(nlohmann::json &j, const User &user);

    void from_json(const nlohmann::json &j, User &user);
}


#endif //TOMADA_SMART_CONDO_JSONDATA_H
