//
// Created by maikeu on 20/08/2019.
//

#ifndef TOMADA_SMART_CONDO_USAGEDATA_H
#define TOMADA_SMART_CONDO_USAGEDATA_H

#include "projectConfig.h"
#ifdef USER_MANAGEMENT_ENABLED

#include <cstdint>
#include <string>

enum class UsageErrorCode : uint8_t {
    None = 0,
    CurrentLimit = 1
};

class UsageData {
public:
    uint32_t TotalAccumulatedEnergy;
    uint32_t AccumulatedEnergy;
    uint32_t InstantPower;
    char User[16]{};
    uint8_t PlugStatus;
    UsageErrorCode ErrorCode = UsageErrorCode::None;

    UsageData(uint32_t totalAccumulatedEnergy, uint32_t accumulatedEnergy, uint32_t instantPower, const char *user,
              uint8_t plugStatus, UsageErrorCode errorCode);

    auto ToJson() -> std::string;

    auto ToJsonChanged(UsageData lastUsage) -> std::string;
};

//void to_json(json &j, const UsageData &usageData);
//void from_json(const json &j, UsageData &usageData);

#endif

#endif //TOMADA_SMART_CONDO_USAGEDATA_H
