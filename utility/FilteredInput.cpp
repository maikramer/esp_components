//
// Created by maikeu on 16/11/2021.
//

#include "FilteredInput.h"

uint32_t FilteredInputEx::_taskStack = 4096;
TaskHandle_t FilteredInputEx::_inputTask = nullptr;
std::list<FilteredInputEx *> FilteredInputEx::_instances{};

void FilteredInputTask(void *none) {
    for (;;) {
        auto instances = FilteredInputEx::GetInstances();
        for (auto input: instances) {
            input->ExUpdate();
            vTaskDelay(10);
        }
    }
    vTaskDelete(nullptr);
}

FilteredInputEx::FilteredInputEx(std::function<uint32_t()> readFunction, uint16_t debounce_ms)
        : FilteredInput(
        readFunction, debounce_ms) {
    if (_inputTask == nullptr) {
        StartTask();
    }

    _instances.push_back(this);

}

void FilteredInputEx::ExUpdate() {
    Update();
    if (!IsFiltered()) return;
    auto now = pdTICKS_TO_MS(xTaskGetTickCount());
    if (_lastState == (ActiveLow ? 0x1 : 0x0) && GetValue() == (ActiveLow ? 0x0 : 0x1) &&
        !_pressedFired) {
        PressedEvent.FireEvent(this, nullptr);
        _pressedFired = true;
        _clickedDetectStart = now;
        ESP_LOGI(__FUNCTION__, "Pressed");
    } else if (_lastState == (ActiveLow ? 0x0 : 0x1) && GetValue() == (ActiveLow ? 0x1 : 0x0) &&
               !_releasedFired) {
        ReleasedEvent.FireEvent(this, nullptr);
        _releasedFired = true;
        if (now - _clickedDetectStart < 1000 && !_clickedFired) {
            ClickedEvent.FireEvent(this, nullptr);
            _clickedFired = true;
        }
    } else {
        _releasedFired = false;
        _pressedFired = false;
        _clickedFired = false;
    }

    _lastState = GetValue();
}

void FilteredInput::Update() {
    auto value = _readFunction();
    if (value == _lastInput) {
        _debounce -= xTaskGetTickCount() - _lastRead;
        _filtered = _debounce < 0;
    } else {
        _filtered = false;
        _debounce = _debounceTime;
    }
    _lastInput = value;
    _lastRead = xTaskGetTickCount();
}
