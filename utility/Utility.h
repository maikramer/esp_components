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

class Utility {


public:
    static auto split(const std::string &source, char delimiter) -> std::vector<std::string>;

    static auto CreateAndProfile(const char *taskName, TaskFunction_t function, uint32_t stack, UBaseType_t priority,
                                 int core,
                                 void *parameter) -> TaskHandle_t;

    static auto StringToByteArray(const std::string &input, uint8_t *output) -> uint32_t;

    static void SetInput(gpio_num_t gpioNum, gpio_pullup_t pullUp, gpio_int_type_t intType = GPIO_INTR_DISABLE);

    static void SetOutput(gpio_num_t gpioNum, bool openDrain, uint32_t initial_level = 0);

    auto Uint64ToSring(uint64_t number) -> string;
};


#endif //TOMADA_SMART_CONDO_UTILITY_H
