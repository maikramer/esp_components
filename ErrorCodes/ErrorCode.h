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

//#define LOGGING_ERROR_CODE
enum class ErrorCodeType {
    General,
    User,
    Storage,
    Project,
    Communication
};
struct ErrorCodeItem {
    const char *Name;
    const char *Description;
    const ErrorCodeType Type;
};
namespace ErrorCodes {
    const ErrorCodeItem Invalid{"Invalid", "Erro Invalido", ErrorCodeType::General};
    const ErrorCodeItem None{"None", "Nenhum Erro Ocorreu", ErrorCodeType::General};
    const ErrorCodeItem Error{"Error", "Erro Inesperado Ocorreu", ErrorCodeType::General};
    const ErrorCodeItem ExceptionError{"ExceptionError", "Erro de Programa", ErrorCodeType::General};
}
class ErrorCode {
private:
    static std::map<std::string, const ErrorCodeItem *> items;
    static bool _initialized;

protected:
    const ErrorCodeItem *_error = nullptr;

public:
    [[nodiscard]] const char *ToString() const {
        return _error->Description;
    }

    [[nodiscard]] const char *GetDescription() const {
        return _error->Description;
    }

    [[nodiscard]] const char *GetName() const {
        return _error->Name;
    }

    static bool AddErrorItem(const ErrorCodeItem &item);

    bool operator==(const ErrorCodeItem item) const {
#ifdef LOGGING_ERROR_CODE
        ESP_LOGI(__FUNCTION__, "Comparando %s == %s", _error->Name, item.Name);
#endif
        return strcmp(_error->Name, item.Name) == 0;
    }

    bool operator!=(const ErrorCodeItem item) const {
#ifdef LOGGING_ERROR_CODE
        ESP_LOGI(__FUNCTION__, "Comparando %s != %s", _error->Name, item.Name);
#endif
        return strcmp(_error->Name, item.Name) != 0;
    }

    bool operator==(const ErrorCode item) const {
#ifdef LOGGING_ERROR_CODE
        ESP_LOGI(__FUNCTION__, "Comparando %s == %s", _error->Name, item._error->Name);
#endif
        return strcmp(_error->Name, item._error->Name) == 0;
    }

    bool operator!=(const ErrorCode item) const {
#ifdef LOGGING_ERROR_CODE
        ESP_LOGI(__FUNCTION__, "Comparando %s != %s", _error->Name, item._error->Name);
#endif
        return strcmp(_error->Name, item._error->Name) != 0;
    }

    explicit operator char *() { return const_cast<char *>(_error->Description); }

    static void Init();

    ErrorCode(ErrorCodeItem codeItem);//NOLINT
};

#endif //TOMADA_SMART_CONDO_ERRORCODE_H
