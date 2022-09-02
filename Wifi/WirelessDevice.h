//
// Created by maikeu on 27/06/22.
//

#include "nlohmann/json.hpp"
#include "JsonModels.h"

#ifndef WIRELESSDEVICE_H
#define WIRELESSDEVICE_H


class WirelessDevice : public JsonModels::BaseJsonData {
public:
    std::string Ssid;
    int Rssi;
    uint Channel;

    WirelessDevice(const std::string &ssid, int rssi, uint channel) : Ssid(ssid), Rssi(rssi),
                                                                      Channel(channel) {}

    WirelessDevice() = default;


    [[nodiscard]] std::string ToJson() const override {
        return ToPureJson().dump();
    }

    [[nodiscard]] std::string ToString() const {
        std::stringstream stream;
        stream << "SSID: " << Ssid << "RSSI: " << Rssi
               << "Canal: "
               << Channel;
        return stream.str();
    }

    [[nodiscard]] nlohmann::json ToPureJson() const {
        nlohmann::json j;
        j["Ssid"] = Ssid;
        j["Rssi"] = Rssi;
        j["Channel"] = Channel;
        return j;
    }

    bool FromJson(const nlohmann::json &j) override {
        if (j.is_null()) return false;
        try {
            Ssid = j["Ssid"];
            Rssi = j["Rssi"];
            Channel = j["Channel"];
        } catch (nlohmann::json::exception &e) {
            ESP_LOGE(__FUNCTION__, "WirelessDevice:Exception: %s", e.what());
            return false;
        } catch (...) {
            return false;
        }

        return true;
    }
};

class ScanForWifiListJsonData : public JsonModels::BaseListJsonData<std::string, WirelessDevice> {
public:
    std::string Ssid = "";
    WirelessDevice WirelessDeviceJson{};

    ScanForWifiListJsonData(const std::string &ssid, const WirelessDevice &wirelessDeviceJson)
            : Ssid(ssid), WirelessDeviceJson(wirelessDeviceJson) {}

    ScanForWifiListJsonData() = default;

    [[nodiscard]] std::string ToJson() const override {
        auto j = GetPartialListJson();
        j["Ssid"] = Ssid;
        j["WirelessDeviceJson"] = WirelessDeviceJson.ToPureJson();

        return j.dump();
    }

    void FromPair(std::string ssid, WirelessDevice wirelessDevice) override {
        Ssid = ssid;
        WirelessDeviceJson = wirelessDevice;
    }
};


void from_json(const nlohmann::json &j, WirelessDevice &device);


#endif //WIRELESSDEVICE_H
