//
// Created by maikeu on 03/07/2020.
//

#ifndef JSONMODELS_H
#define JSONMODELS_H

#include <cstring>
#include <nlohmann/json.hpp>
#include <HX711.h>

namespace JsonModels {
    enum class SimpleErrorCode : uint8_t {
        None = 0,
        Error = 1
    };

    class BaseJsonData {
    };

    class UuidInfoJsonData : public BaseJsonData {

    public:
        std::string NotifyUUID = "";
        std::string ServiceUUID = "";
        std::string WriteUUID = "";

        auto ToJson() -> std::string {
            nlohmann::json j;
            j["NotifyUUID"] = NotifyUUID;
            j["ServiceUUID"] = ServiceUUID;
            j["WriteUUID"] = WriteUUID;
            return j.dump();
        }
    };

    class UpdateDataJson : BaseJsonData {
    protected:
        bool IsUpdate = true;

        auto GetPartialUpdateJson() -> nlohmann::json {
            nlohmann::json j;
            j["IsUpdate"] = IsUpdate;
            return j;
        }
    };

    class BaseJsonDataError : public BaseJsonData {
    public:
        uint8_t ErrorCode = 0;

        auto ToBaseJson() -> std::string {
            auto j = GetPartialJson(true);
            return j.dump();
        }

    protected:
        auto GetPartialJson(bool force) -> nlohmann::json {
            nlohmann::json j;
            if (ErrorCode != 0 || force) {
                j["Error"] = ErrorCode != 0;
                j["ErrorCode"] = ErrorCode;
            }
            return j;
        }

    };

    class BaseListJsonData : public BaseJsonDataError {

    public:
        bool End = false;
        bool Begin = false;

    protected:
        auto GetPartialListJson() -> nlohmann::json {
            auto j = GetPartialJson(false);
            if (Begin) {
                j["Begin"] = Begin;
            } else if (End) {
                j["End"] = End;
            }
            return j;
        }
    };


}
#endif //JSONMODELS_H
