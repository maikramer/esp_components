//
// Created by maikeu on 17/07/2020.
//

#ifndef TOMADA_SMART_CONDO_ERRORCODE_H
#define TOMADA_SMART_CONDO_ERRORCODE_H


#include <cstdint>
#include <list>
#include <Utility.h>
#include <esp_log.h>
#include "string"
#include "nameof.hpp"

namespace ErrorCodes {
    constexpr uint8_t None = 0;
    constexpr uint8_t Error = 1;
    constexpr uint8_t StoreError = 2;
    constexpr uint8_t ListIsEmpty = 3;
    constexpr uint8_t Exist = 4;
    constexpr uint8_t FileNotFound = 5;
    constexpr uint8_t KeyNotFound = 6;
    constexpr uint8_t FindError = 7;
    constexpr uint8_t PartitionNotFound = 8;
    constexpr uint8_t AlreadyMounted = 9;
    constexpr uint8_t NoFreeMemory = 10;
}

class ErrorCodeItem {
public:
    static const ErrorCodeItem Invalid;
    uint8_t _code;
    std::string _name;

    ErrorCodeItem(uint8_t code, std::string name) : _code(code) {
        _name = Utility::CamelCaseToTitleCase(name);
        ESP_LOGI(__FUNCTION__, "Adicionado erro: %s", _name.c_str());
    }
};

class ErrorCode {
private:
    static std::list<ErrorCodeItem> items;
    static bool _init;

    static void Init() {
        AddErrorItem(ErrorCodeItem(ErrorCodes::None, std::string(NAMEOF(ErrorCodes::None))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::Error, std::string(NAMEOF(ErrorCodes::Error))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::StoreError, std::string(NAMEOF(ErrorCodes::StoreError))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::ListIsEmpty, std::string(NAMEOF(ErrorCodes::ListIsEmpty))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::Exist, std::string(NAMEOF(ErrorCodes::Exist))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::FileNotFound, std::string(NAMEOF(ErrorCodes::FileNotFound))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::KeyNotFound, std::string(NAMEOF(ErrorCodes::KeyNotFound))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::FindError, std::string(NAMEOF(ErrorCodes::FindError))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::PartitionNotFound, std::string(NAMEOF(ErrorCodes::PartitionNotFound))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::AlreadyMounted, std::string(NAMEOF(ErrorCodes::AlreadyMounted))));
        AddErrorItem(ErrorCodeItem(ErrorCodes::NoFreeMemory, std::string(NAMEOF(ErrorCodes::NoFreeMemory))));
    }

protected:
    ErrorCodeItem *Error = nullptr;

    explicit ErrorCode(ErrorCodeItem *item) {
        Error = item;
    }

public:
    std::string ToString() {
        return Error->_name;
    }

    static bool AddErrorItem(const ErrorCodeItem &item) {
        for (const auto &it:items) {
            if (it._name == item._name || it._code == item._code) {
                ESP_LOGE(__FUNCTION__, "Tentando adicionar codigo de erro ja existente");
                return false;
            }
        }
        items.push_back(item);
    }

    bool operator==(uint8_t value) const {
        return value == Error->_code;
    }

    bool operator!=(uint8_t value) const {
        return value != Error->_code;
    }

    explicit operator uint8_t() { return Error->_code; }

    static bool IsValid(uint8_t code) {
        for (auto &item:items) {
            if (item._code == code) {
                return true;
            }
        }
        ESP_LOGE(__FUNCTION__, "Codigo de erro invalido");
        return false;
    }

    static ErrorCodeItem *GetByCode(uint8_t code) {
        if (items.size() < code) {
            ESP_LOGE(__FUNCTION__, "Codigo invalido %d", code);
        }
        for (auto &item:items) {
            if (item._code == code) {
                return &item;
            }
        }
        ESP_LOGE(__FUNCTION__, "Codigo %d nao encontrado", code);
        return const_cast<ErrorCodeItem *>(&ErrorCodeItem::Invalid);
    }

    ErrorCode(uint8_t code) {
        Error = GetByCode(code);
    }

    ErrorCode() {
        Error = GetByCode(ErrorCodes::None);
    }
};

#endif //TOMADA_SMART_CONDO_ERRORCODE_H
