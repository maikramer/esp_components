//
// Created by maikeu on 17/07/2020.
//

#include "ErrorCode.h"

std::map<std::string, const ErrorCodeItem *>ErrorCode::items{};
bool ErrorCode::_initialized = false;

ErrorCode::ErrorCode(const ErrorCodeItem item) {
    if (items.find(item.Name) == items.end()) {
        ESP_LOGE(__FUNCTION__, "Item de erro < %s > não está no banco de dados", item.Name);
        _error = &ErrorCodes::Invalid;
        return;
    }

    _error = items[item.Name];
}

bool ErrorCode::AddErrorItem(const ErrorCodeItem &item) {
    if (items.find(item.Name) != items.end()) {
        ESP_LOGE(__FUNCTION__, "Tentando adicionar %s, que já existe", item.Name);
        return false;
    }
    items[item.Name] = &item;
    return true;
}

void ErrorCode::Init() {
    if(_initialized) return;
    AddErrorItem(ErrorCodes::Invalid);
    AddErrorItem(ErrorCodes::None);
    AddErrorItem(ErrorCodes::Error);
    AddErrorItem(ErrorCodes::ExceptionError);
    _initialized = true;
}
