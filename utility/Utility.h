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
#include <JsonModels.h>
#include "esp_log.h"
#include "stdexcept"
#include "sstream"

class Utility {


public:
    static auto split(const std::string &source, char delimiter) -> std::vector<std::string>;

    static auto CreateAndProfile(const char *taskName, TaskFunction_t function, uint32_t stack, UBaseType_t priority,
                                 int core,
                                 void *parameter) -> TaskHandle_t;

    static auto StringToByteArray(const std::string &input, uint8_t *output) -> uint32_t;

    static void SetInput(gpio_num_t gpioNum, gpio_pullup_t pullUp, gpio_int_type_t intType = GPIO_INTR_DISABLE);

    static void SetOutput(gpio_num_t gpioNum, bool openDrain, uint32_t initial_level = 0);

    static std::string CamelCaseToTitleCase(const std::string &toConvert);

    template<typename T>
    static auto GetConvertedFromString(std::string str) -> T {
        T out;
        if (str.empty()) {
            ESP_LOGE(__FUNCTION__, "String vazia");
        }
        ESP_LOGI(__FUNCTION__, "Tentando converter %s", str.c_str());
        std::stringstream convert(str);
        convert >> out;

        return out;
    }

    template<typename T>
    struct memfun_type {
        using type = void;
    };

    template<typename Ret, typename Class, typename... Args>
    struct memfun_type<Ret(Class::*)(Args...) const> {
        using type = std::function<Ret(Args...)>;
    };

    template<typename F>
    typename memfun_type<decltype(&F::operator())>::type
    static FFL(F const &func) { // Function from lambda !
        return func;
    }
};


#endif //TOMADA_SMART_CONDO_UTILITY_H
