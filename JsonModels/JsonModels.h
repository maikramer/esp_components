//
// Created by maikeu on 03/07/2020.
//

#ifndef JSONMODELS_H
#define JSONMODELS_H

#include <string>
#include <nlohmann/json.hpp>
#include "projectConfig.h"
#include "esp_log.h"

namespace JsonModels {
    enum class SimpleErrorCode : uint8_t {
        None = 0,
        Error = 1
    };

    class BaseJsonData {
    public:
        [[nodiscard]] virtual std::string ToJson() const = 0;

        virtual bool FromString(const std::string &str) { return true; };
    };

    std::ostream &operator<<(std::ostream &Str, JsonModels::BaseJsonData const &v);

    class UuidInfoJsonData : public BaseJsonData {

    public:
        std::string NotifyUUID = "";
        std::string ServiceUUID = "";
        std::string WriteUUID = "";

        [[nodiscard]] auto ToJson() const -> std::string override {
            nlohmann::json j;
            j["NotifyUUID"] = NotifyUUID;
            j["ServiceUUID"] = ServiceUUID;
            j["WriteUUID"] = WriteUUID;
            return j.dump();
        }
    };

    class UpdateDataJson : BaseJsonData {
    protected:
        UpdateDataJson() = default;

        bool IsUpdate = true;

        [[nodiscard]] auto GetPartialUpdateJson() const -> nlohmann::json {
            nlohmann::json j;
            j["IsUpdate"] = IsUpdate;
            return j;
        }
    };

    class BaseJsonDataError : public BaseJsonData {
    public:
        uint8_t ErrorCode = 0;

        [[nodiscard]] std::string ToJson() const override {
            auto j = GetPartialJson(true);
            return j.dump();
        }

    protected:
        [[nodiscard]] auto GetPartialJson(bool force) const -> nlohmann::json {
            nlohmann::json j;
            if (ErrorCode != 0 || force) {
                j["Error"] = ErrorCode != 0;
                j["ErrorCode"] = ErrorCode;
            }
            return j;
        }

    };

    class BaseListJsonDataBasic : public BaseJsonDataError {
    public:
        bool End = false;
        bool Begin = false;
    protected:
        [[nodiscard]] nlohmann::json GetPartialListJson() const {
            auto j = GetPartialJson(false);
            if (Begin) {
                j["Begin"] = Begin;
            } else if (End) {
                j["End"] = End;
            }
            return j;
        }

    };

    template<typename Tkey, typename Tvalue>
    class BaseListJsonData : public BaseListJsonDataBasic {

    public:

        virtual void FromPair(Tkey first, Tvalue second) = 0;


    };

#ifdef USER_MANAGEMENT_ENABLED

    class User : public BaseJsonData {
    public:
        std::string Name = "";
        std::string Password = "";
        std::string Email = "";
        bool IsConfirmed = false;

        [[nodiscard]] std::string ToJson() const override {
            return ToPureJson().dump();
        }

        [[nodiscard]] nlohmann::json ToPureJson() const {
            nlohmann::json j;
            j["Name"] = Name;
            j["Password"] = Password;
            j["Email"] = Email;
            j["IsConfirmed"] = IsConfirmed;
            return j;
        }

        bool FromString(const std::string &jsonStr) override {
            try {
                nlohmann::json j = nlohmann::json::parse(jsonStr);
                Name = j["Name"];
                Password = j["Password"];
                Email = j["Email"];
                IsConfirmed = j["IsConfirmed"];
            } catch (nlohmann::json::exception &e) {
                ESP_LOGE(__FUNCTION__, "Exception: %s", e.what());
                return false;
            } catch (...) {
                return false;
            }

            return true;
        }
    };

    class UserListJsonData : public BaseListJsonData<std::string, User> {
    public:
        std::string UserName = "";
        User UserJson;

        [[nodiscard]] std::string ToJson() const override {
            auto j = GetPartialListJson();
            if (Begin) {
                j["UserName"] = UserName;
            }
            j["UserJson"] = UserJson.ToPureJson();

            return j.dump();
        }

        void FromPair(std::string userName, User userJson) override {
            UserName = userName;
            UserJson = userJson;
        }
    };

#endif

}

#endif //JSONMODELS_H
