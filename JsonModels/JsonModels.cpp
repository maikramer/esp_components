//
// Created by maikeu on 03/07/2020.
//

#include "JsonModels.h"

std::ostream &JsonModels::operator<<(std::ostream &Str, const JsonModels::BaseJsonData &v) {
    Str << v.ToJson();
    return Str;
}

std::istream &JsonModels::operator>>(std::istream &in, JsonModels::BaseJsonData &v) {
    std::string str;
    in >> str;
    v.FromString(str);
    return in;
}

auto JsonModels::BaseJsonDataError::GetPartialJson(bool force) const -> nlohmann::json {
    nlohmann::json j;
    //Um placeholder para não enviar um json vazio nunca
    j[""] = "";
    if (ErrorMessage != ErrorCodes::None || force) {
        j["Error"] = ErrorMessage != ErrorCodes::None;
        j["ErrorMessage"] = ErrorMessage.GetDescription();
        j["ErrorName"] = ErrorMessage.GetName();
    }
    return j;
}
#ifdef USER_MANAGEMENT_ENABLED
std::string JsonModels::LoginTryResultJson::ToJson() const {
    auto j = GetPartialJson(false);
    if (IsAdmin) {
        j["IsAdmin"] = IsAdmin;
    }

    return j.dump();
}
#endif
