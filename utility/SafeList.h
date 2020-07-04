//
// Created by maikeu on 13/02/2020.
//

#ifndef TOMADA_SMART_CONDO_SAFELIST_H
#define TOMADA_SMART_CONDO_SAFELIST_H

#include <list>
#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include <freertos/task.h>


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

    bool CheckBusy() {
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
        if (xSemaphoreTake(xSemaphore, TIMEOUT) == pdTRUE) {
            return _list;
        }
        return std::list<T>();
    }

    void EndReadList() {
        xSemaphoreGive(xSemaphore);
    }

    void Remove(T item) {

        if (xSemaphoreTake(xSemaphore, TIMEOUT) == pdTRUE) {
            xSemaphoreTake(xSemaphore, TIMEOUT);
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


#endif //TOMADA_SMART_CONDO_SAFELIST_H
