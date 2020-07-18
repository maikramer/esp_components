//
// Created by maikeu on 17/07/2020.
//

#include "ErrorCode.h"

std::map<const char *, const ErrorCodeItem *, StrCompare>ErrorCode::items{};

ErrorCode::ErrorCode(const ErrorCodeItem item) {
    if (items.find(item.Name) == items.end()) {
        ESP_LOGE(__FUNCTION__, "Item de erro < %s > não está no banco de dados", item.Name);
        _error = &ErrorCodes::Invalid;
        return;
    }

    _error = items[item.Name];
}
