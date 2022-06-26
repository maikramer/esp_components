#include "Event.h"

auto NakedEvent::AddListener(std::function<void()> func) -> int {
    int handler = counter;
    if (xSemaphoreTake(xSemaphore, MAX_DELAY) == pdPASS) {
        listeners.insert(std::pair<int, std::function<void()>>(handler, func));
        xSemaphoreGive(xSemaphore);
    }
    counter++;

    return handler;
}

void NakedEvent::RemoveListener(int handle) {
    if (xSemaphoreTake(xSemaphore, MAX_DELAY) == pdPASS) {
        listeners.erase(handle);
        xSemaphoreGive(xSemaphore);
    }
}

void NakedEvent::FireEvent() {
    if (xSemaphoreTake(xSemaphore, MAX_DELAY) == pdPASS) {
        for (auto const &f: listeners) {
            f.second();
        }
        xSemaphoreGive(xSemaphore);
    }
}

