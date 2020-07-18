//
// Created by maikeu on 03/07/2020.
//

#include "JsonModels.h"

std::ostream &JsonModels::operator<<(std::ostream &Str, const JsonModels::BaseJsonData &v) {
    // print something from v to str, e.g: Str << v.getX();
    Str << v.ToJson();
    return Str;
}

auto JsonModels::BaseJsonDataError::GetPartialJson(bool force) const -> nlohmann::json {
    nlohmann::json j;
    if (ErrorMessage != ErrorCodes::None || force) {
        j["Error"] = ErrorMessage != ErrorCodes::None;
        j["ErrorMessage"] =  ErrorMessage.ToString();
    }
    return j;
}
