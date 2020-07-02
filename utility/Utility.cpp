//
// Created by maikeu on 02/02/2020.
//

#include <vector>
#include <sstream>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <esp_log.h>
#include "Utility.h"
#include <GeneralUtils.h>

//#define PROFILE_STACK

auto Utility::split(const std::string &source, char delimiter) -> std::vector<std::string> {
    std::vector<std::string> strings;
    std::istringstream iss(source);
    std::string s;

    if (iss.peek() == '{') {
        std::string json;
        std::getline(iss, json);
        strings.push_back(json);
        return strings;
    }

    while (std::getline(iss, s, delimiter)) {
        strings.push_back(GeneralUtils::trim(s));
    }

    return strings;
} // split

auto Utility::StringToByteArray(const std::string &input, uint8_t *output) -> uint32_t {
    std::copy(input.begin(), input.end(), output);
    return input.size();
}

auto Utility::CreateAndProfile(const char *taskName, TaskFunction_t function, const uint32_t stack,
                               UBaseType_t priority,
                               int core, void *parameter) -> TaskHandle_t {
    xTaskHandle handle;
    auto res = xTaskCreatePinnedToCore(function, taskName, stack, parameter, priority, &handle, core);
    configASSERT(handle)
    if (res != pdPASS || handle == nullptr) {
        ESP_LOGE(__FUNCTION__, "Falha ao criar a tarefa \"%s\"", taskName);
    } else {
        ESP_LOGI(__FUNCTION__, "Tarefa \"%s\" criada com sucesso", taskName);
    }

#ifdef PROFILE_STACK
    vTaskDelay(5);
    auto stackSize = uxTaskGetStackHighWaterMark(handle);
    ESP_LOGI(__FUNCTION__, "%s stack: %d\n", taskName, stackSize);
    vTaskDelay(5);
#endif

    return handle;
}

//auto Utility::CreateAndProfileStatic(const char *taskName, TaskFunction_t function, const uint32_t stack,
//                                     UBaseType_t priority, StackType_t *const pxStackBuffer,
//                                     StaticTask_t *const pxTaskBuffer,
//                                     BaseType_t core, void *parameter) -> TaskHandle_t {
//    xTaskHandle handle;
//    handle = xTaskCreateStaticPinnedToCore(function, taskName, stack, parameter, priority, pxStackBuffer,
//                                           pxTaskBuffer, core);
//    ESP_LOGI(__FUNCTION__, "Tarefa \"%s\" criada com sucesso", taskName);
//
//#ifdef PROFILE_STACK
//    vTaskDelay(5);
//    auto stackSize = uxTaskGetStackHighWaterMark(handle);
//    ESP_LOGI(__FUNCTION__, "%s stack: %d\n", taskName, stackSize);
//    vTaskDelay(5);
//#endif
//
//    return handle;
//}