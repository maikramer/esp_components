/*
 * FilteredInput.h
 *
 *  Created on: Jul 10, 2021
 *      Author: maikeu
 */

#ifndef INC_FILTEREDINPUT_H_
#define INC_FILTEREDINPUT_H_

#include <functional>
#include <Event.h>
#include <Utility.h>
#include <priorities.h>

#ifdef STM32L1
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#elif defined(ESP_PLATFORM)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#endif

void FilteredInputTask(void *filteredInput);

class FilteredInput {
public:
    FilteredInput(std::function<uint32_t()> readFunction, uint16_t debounce_ms) {
        _readFunction = readFunction;
        _debounceTime = pdMS_TO_TICKS(debounce_ms);
        _debounce = _debounceTime;
    }

    bool IsFiltered() {
        Update();
        return (_filtered);
    }

    uint32_t GetValue() {
        Update();
        return (_lastInput);
    }

    bool Changed() {
        Update();
        auto ret = _lastChangedCall != _lastInput;
        _lastChangedCall = _lastInput;
        return (ret);
    }

    void Update();


private:

    std::function<uint32_t()> _readFunction;
    bool _filtered = false;
    int32_t _debounce = 0;
    int32_t _debounceTime = 0;
    uint32_t _lastInput = 0;
    TickType_t _lastRead = 0;
    uint32_t _lastChangedCall = 0;
};


class FilteredInputEx : public FilteredInput {
public:
    FilteredInputEx(std::function<uint32_t()> readFunction, uint16_t debounce_ms);

    Event<FilteredInput *, void *> PressedEvent{};
    Event<FilteredInput *, void *> ReleasedEvent{};
    Event<FilteredInput *, void *> ClickedEvent{};
    uint32_t _clickedDetectStart;

    void ExUpdate();

    static void SetTaskStackSize(uint32_t stackSize) {
        _taskStack = stackSize;
        if (_inputTask != nullptr) {
            vTaskDelete(_inputTask);
            StartTask();
        }
    }

    static std::list<FilteredInputEx *> GetInstances() {
        return _instances;
    }

    bool ActiveLow = true;
private:

    static void StartTask() {
        Utility::CreateAndProfile("FilteredInputTask", FilteredInputTask, _taskStack, HIGH_PRIORITY,
                                  1,
                                  nullptr);
    }

    uint32_t _lastState;
    bool _pressedFired = false;
    bool _releasedFired = false;
    bool _clickedFired = false;
    static uint32_t _taskStack;
    static TaskHandle_t _inputTask;
    static std::list<FilteredInputEx *> _instances;
};


#endif /* INC_FILTEREDINPUT_H_ */
