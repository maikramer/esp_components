//
// Created by maikeu on 13/02/2020.
//

#ifndef SAFELIST_H
#define SAFELIST_H

#ifdef STM32L1

#include <FreeRTOS.h>
#include <semphr.h>

#define MEDIUM_PRIORITY 4
#elif defined(ESP_PLATFORM)

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>

#define DIO1_Pin GPIO_NUM_34
#endif

#include <list>

template<class T>
class SafeList {
public:
    SafeList() = default;

    auto Empty() -> bool {
        if (xSemaphoreTake(xSemaphore, TIMEOUT) == pdTRUE) {
            bool empty = _list.empty();
            xSemaphoreGive(xSemaphore);
            return empty;
        }
        return true;

    }

    void PopFront() {
        if (xSemaphoreTake(xSemaphore, TIMEOUT) == pdTRUE) {
            _list.pop_front();
            xSemaphoreGive(xSemaphore);
        }
    }

    auto CheckBusy() -> bool {
        bool busy = (xSemaphoreTake(xSemaphore, TIMEOUT) != pdTRUE);
        if (!busy) {
            xSemaphoreGive(xSemaphore);
        }

        return busy;
    }

    void Push(T item) {
        if (xSemaphoreTake(xSemaphore, TIMEOUT) == pdTRUE) {
            _list.push_back(item);
            xSemaphoreGive(xSemaphore);
        }
    }

    auto ReadList() -> std::list<T> {
//        ESP_LOGI(__FUNCTION__, "Abrindo lista, nao esqueca de fechar");
        if (xSemaphoreTake(xSemaphore, TIMEOUT) == pdTRUE) {
            return _list;
        }
        return std::list<T>();
    }

    void EndReadList() {
//        ESP_LOGI(__FUNCTION__, "Fechando lista");
        xSemaphoreGive(xSemaphore);

    }

    void Remove(T item) {
        if (xSemaphoreTake(xSemaphore, TIMEOUT) == pdTRUE) {
            _list.remove(item);
            xSemaphoreGive(xSemaphore);
        }

    }

    auto Size() -> uint32_t {
        if (xSemaphoreTake(xSemaphore, TIMEOUT) == pdTRUE) {
            uint32_t size = _list.size();
            xSemaphoreGive(xSemaphore);
            return size;
        }
        return 0;
    }

private:
    const int TIMEOUT = 100;
    std::list<T> _list = std::list<T>();
    SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
};

#endif //SAFELIST_H
