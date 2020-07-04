//
// Created by maikeu on 15/08/2019.
//

#ifndef ROCKET_TESTER_FLASH_H
#define ROCKET_TESTER_FLASH_H

#include <map>
#include <list>
#include <string>
#include "Storage.h"

enum class DataType {
    String,
    Int64,
    Uint64,
    Int32,
    Uint32,
    Int8,
    Uint8
};

class Flash {
public:
    static void Init();

    static auto StoreConfig(const std::string &key, std::string &value, bool overwrite) -> StoreResult;

    static auto
    StoreKeyValue(const std::string &key, std::string &str, const std::string &fileName, bool overwrite) -> StoreResult;

    static auto
    StoreKeyValue(const std::string &key, uint32_t value, const std::string &fileName, bool overwrite) -> StoreResult;

    static auto
    StoreKeyValue(const std::string &key, int32_t value, const std::string &fileName, bool overwrite) -> StoreResult;

    static auto ReadKeyValue(const std::string &key, std::string &out, const std::string &fileName) -> StoreResult;

    static auto ReadKeyValue(const std::string &key, uint32_t *out, const std::string &fileName) -> StoreResult;

    auto LoadConfig(const std::string &key, std::string &config) -> StoreResult;

private:
    static auto StoreKeyValue(const std::string &key, void *value, const std::string &fileName, DataType type,
                              bool overwrite) -> StoreResult;

    static auto
    ReadKeyValue(const std::string &key, void *out, const std::string &fileName, DataType type) -> StoreResult;

};


#endif //ROCKET_TESTER_FLASH_H
