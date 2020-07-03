//
// Created by maikeu on 20/08/2019.
//
#include "projectConfig.h"
#ifdef USER_MANAGEMENT_ENABLED

#include <cstring>
#include "UsageData.h"
#include <nlohmann/json.hpp>

UsageData::UsageData(uint32_t totalAccumulatedEnergy, uint32_t accumulatedEnergy, uint32_t instantPower,
                     const char *user, uint8_t plugStatus, UsageErrorCode errorCode)
        : TotalAccumulatedEnergy(totalAccumulatedEnergy), AccumulatedEnergy(accumulatedEnergy),
          InstantPower(instantPower), PlugStatus(plugStatus), ErrorCode(errorCode) {
    strcpy(User, user);
}

auto UsageData::ToJson() -> std::string {

    nlohmann::json j;
    j["TotalAccumulatedEnergy"] = TotalAccumulatedEnergy;
    j["AccumulatedEnergy"] = AccumulatedEnergy;
    j["InstantPower"] = InstantPower;
    j["User"] = User;
    j["PlugStatus"] = PlugStatus;

    if (ErrorCode != UsageErrorCode::None) {
        j["ErrorCode"] = (uint8_t) ErrorCode;
    }

    return j.dump();
}

auto UsageData::ToJsonChanged(UsageData lastUsage) -> std::string {

    nlohmann::json j;
    if (lastUsage.TotalAccumulatedEnergy != TotalAccumulatedEnergy) {
        j["TotalAccumulatedEnergy"] = TotalAccumulatedEnergy;
    }
    if (lastUsage.AccumulatedEnergy != AccumulatedEnergy) {
        j["AccumulatedEnergy"] = AccumulatedEnergy;
    }
    if (lastUsage.InstantPower != InstantPower) {
        j["InstantPower"] = InstantPower;
    }

    j["User"] = User;

    if (lastUsage.PlugStatus != PlugStatus) {
        j["PlugStatus"] = PlugStatus;
    }

    if (ErrorCode != UsageErrorCode::None) {
        j["ErrorCode"] = (uint8_t) ErrorCode;
    }

    return j.dump();
}

#endif