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
        xSemaphoreTake(xSemaphore, TIMEOUT);
        bool empty = _list.empty();
        xSemaphoreGive(xSemaphore);
        return empty;
    }

    void Push(T item) {
        xSemaphoreTake(xSemaphore, TIMEOUT);
        _list.push_back(item);
        xSemaphoreGive(xSemaphore);
    }

    auto ReadList() -> std::list<T> {
        xSemaphoreTake(xSemaphore, TIMEOUT);
        return _list;
    }

    void EndReadList() {
        xSemaphoreGive(xSemaphore);
    }

    void Remove(T item) {

        xSemaphoreTake(xSemaphore, TIMEOUT);
        _list.remove(item);

        xSemaphoreGive(xSemaphore);

    }

    auto Size() -> uint32_t {
        return _list.size();
    }

private:
    const int TIMEOUT = 100;
    std::list<T> _list = std::list<T>();
    SemaphoreHandle_t xSemaphore = xSemaphoreCreateMutex();
};


#endif //TOMADA_SMART_CONDO_SAFELIST_H
