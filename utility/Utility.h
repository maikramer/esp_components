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

class Utility {


public:
    static auto split(const std::string &source, char delimiter) -> std::vector<std::string>;

    static auto CreateAndProfile(const char *taskName, TaskFunction_t function, uint32_t stack, UBaseType_t priority,
                                 int core,
                                 void *parameter) -> TaskHandle_t;

    static uint32_t StringToByteArray(const std::string &input, uint8_t *output);
};


#endif //TOMADA_SMART_CONDO_UTILITY_H
