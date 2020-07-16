//
// Created by maikeu on 02/02/2020.
//

#ifndef TOMADA_SMART_CONDO_UTILITY_H
#define TOMADA_SMART_CONDO_UTILITY_H


#include <freertos/projdefs.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <vector>
#include "string"
#include <hal/gpio_types.h>
#include <driver/gpio.h>
#include "esp_log.h"
#include "stdexcept"

class Utility {


public:
    static auto split(const std::string &source, char delimiter) -> std::vector<std::string>;

    static auto CreateAndProfile(const char *taskName, TaskFunction_t function, uint32_t stack, UBaseType_t priority,
                                 int core,
                                 void *parameter) -> TaskHandle_t;

    static auto StringToByteArray(const std::string &input, uint8_t *output) -> uint32_t;

    static void SetInput(gpio_num_t gpioNum, gpio_pullup_t pullUp, gpio_int_type_t intType = GPIO_INTR_DISABLE);

    static void SetOutput(gpio_num_t gpioNum, bool openDrain, uint32_t initial_level = 0);

    auto Uint64ToSring(uint64_t number) -> std::string;

    template<typename T>
    static auto GetConvertedFromString(std::string str) -> T {
        T out;
        try {
            if (std::is_same<T, std::string>()) {
                out = *reinterpret_cast<T *>(&str);
            } else if (std::is_same<T, std::int32_t>()) {
                out = stoi(str);
            } else if (std::is_same<T, std::uint32_t>()) {
                out = stoul(str);
            } else if (std::is_same<T, std::int64_t>()) {
                out = stoll(str);
            } else if (std::is_same<T, std::uint64_t>()) {
                out = stoull(str);
            } else {
                ESP_LOGE(__FUNCTION__, "Tipo invalido ou nao suportado");
            }
        } catch (std::invalid_argument &e) {
            ESP_LOGE(__FUNCTION__, "Excessao: %s", e.what());
            throw std::exception();
        } catch (std::out_of_range &e) {
            ESP_LOGE(__FUNCTION__, "Excessao: %s", e.what());
            throw std::exception();
        }
        return out;
    }
};


#endif //TOMADA_SMART_CONDO_UTILITY_H
