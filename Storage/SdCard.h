//
// Created by maikeu on 23/09/2019.
//

#ifndef ROCKET_TESTER_SDCARD_H
#define ROCKET_TESTER_SDCARD_H
#ifdef USE_SDCARD

#include <Storage.h>
#include <JsonData.h>
#include <custom/FreeRTOS.h>
#include <freertos/semphr.h>
#include <map>
#include <esp_log.h>
#ifdef USER_MANAGEMENT_ENABLED
typedef bool (boolFunctionPointerUser)(const JsonData::User &user);
#endif
typedef bool (boolFunctionPointer)();

class SdCard {
public:

    static auto Init() -> bool;

    static auto
    FastStoreKeyValue(const std::string &key, const std::string &value, const std::string &fileName) -> bool;

    static auto StoreConfig(const std::string &key, const std::string &value, bool overwrite) -> StoreResult;

    static auto LoadConfig(const std::string &key) -> std::string;

    static auto StoreKeyValue(const std::string &key, const std::string &value, const std::string &fileName,
                              bool overwrite) -> StoreResult;

    static auto ReadKeyFromFile(const std::string &key, const std::string &fileName) -> std::string;
#ifdef USER_MANAGEMENT_ENABLED
    static auto GetEntriesFromUser(const std::string &user, std::map<uint32_t, uint32_t> &map) -> bool;
    static auto GetUserListWithFilter(boolFunctionPointerUser filter) -> std::list<JsonData::User>;
    static auto LoadUserJson(const std::string &username) -> std::string;
    static auto
    StoreUserJson(const std::string &username, const std::string &userJson, bool overwrite) -> StoreResult;

    static auto GetEntriesWithFilter(const std::string &user, std::map<uint32_t, uint32_t> &map,
                                     boolFunctionPointer filter) -> bool;

    static auto GetAccumulatedFromUser(const std::string &apTorre, uint32_t from, uint32_t to) -> uint32_t;
#endif
};

#endif
#endif //ROCKET_TESTER_SDCARD_H
