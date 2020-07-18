//
// Created by maikeu on 17/07/2020.
//

#ifndef TOMADA_SMART_CONDO_ERRORCODE_H
#define TOMADA_SMART_CONDO_ERRORCODE_H


#include <cstdint>
#include <map>
#include <esp_log.h>
#include "cstring"
#include "string"
#include <functional>

struct StrCompare : public std::binary_function<const char *, const char *, bool> {
public:
    bool operator()(const char *str1, const char *str2) const { return std::strcmp(str1, str2) < 0; }
};

struct ErrorCodeItem {
    const char *Name;
    const char *Description;
};
namespace ErrorCodes {
    const ErrorCodeItem Invalid{"Invalid", "Erro Invalido"};
    const ErrorCodeItem None{"None", "Nenhum Erro Ocorreu"};
    const ErrorCodeItem Error{"Error", "Erro Inesperado Ocorreu"};
    const ErrorCodeItem ExceptionError{"ExceptionError", "Erro de Programa"};
}
class ErrorCode {
private:
    static std::map<const char *, const ErrorCodeItem *, StrCompare> items;

protected:
    const ErrorCodeItem *_error = nullptr;

public:
    [[nodiscard]] const char *ToString() const {
        return _error->Description;
    }

    static bool AddErrorItem(const ErrorCodeItem &item) {
        if (items.find(item.Name) != items.end()) {
            ESP_LOGE(__FUNCTION__, "Tentando adicionar erro ja existente");
            return false;
        }
        items[item.Name] = &item;
        return true;
    }

    bool operator==(const ErrorCodeItem item) const {
        ESP_LOGI(__FUNCTION__, "Comparando %s == %s", _error->Name, item.Name);
        return strcmp(_error->Name, item.Name) == 0;
    }

    bool operator!=(const ErrorCodeItem item) const {
        ESP_LOGI(__FUNCTION__, "Comparando %s != %s", _error->Name, item.Name);
        return strcmp(_error->Name, item.Name) != 0;
    }

    explicit operator char *() { return const_cast<char *>(_error->Description); }

    static void Init() {
        AddErrorItem(ErrorCodes::Invalid);
        AddErrorItem(ErrorCodes::None);
        AddErrorItem(ErrorCodes::Error);
        AddErrorItem(ErrorCodes::ExceptionError);
    }

    ErrorCode(ErrorCodeItem codeItem);//NOLINT
};

#endif //TOMADA_SMART_CONDO_ERRORCODE_H
