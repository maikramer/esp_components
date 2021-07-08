//
// Created by maikeu on 04/07/2020.
//

#ifndef EVENT_H
#define EVENT_H

#include <functional>
#include "list"
#include "map"

#ifdef STM32L1
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#elif defined(ESP_PLATFORM)

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#endif

//C - Caller e A o argumento
template<class C, class A>
class Event {
private:
    const int MAX_DELAY = 1000;
    SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
    int counter = 0;
    std::map<int, std::function<void(C, A)>> listeners{};
public:
    // Retorna um handler para remover da lista depois
    auto AddListener(std::function<void(C, A)> func) -> int;

    void RemoveListener(int handle);

    void FireEvent(C caller, A eventArgs);
};

template<class C, class A>
auto Event<C, A>::AddListener(std::function<void(C, A)> func) -> int {
    int handler = counter;
    if (xSemaphoreTake(xSemaphore, MAX_DELAY) == pdPASS) {
        listeners.insert(std::pair<int, std::function<void(C, A)>>(handler, func));
        xSemaphoreGive(xSemaphore);
    }
    counter++;

    return handler;
}

template<class C, class A>
void Event<C, A>::RemoveListener(int handle) {
    if (xSemaphoreTake(xSemaphore, MAX_DELAY) == pdPASS) {
        listeners.erase(handle);
        xSemaphoreGive(xSemaphore);
    }
}

template<class C, class A>
void Event<C, A>::FireEvent(C caller, A eventArgs) {
    if (xSemaphoreTake(xSemaphore, MAX_DELAY) == pdPASS) {
        for (auto const &f : listeners) {
            f.second(caller, eventArgs);
        }
        xSemaphoreGive(xSemaphore);
    }
}


#endif //ROCKET_TESTER_EVENT_H
