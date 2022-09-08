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

#endif

#include <list>
#include "functional"
#include "LockableContainer.h"
#include "CrossPlatformUtility.h"

template<class T>
class SafeList : public LockableContainer {
public:
    SafeList() = default;

    auto Empty() -> bool {
        if (Lock()) {
            bool empty = _list.empty();
            Unlock();
            return empty;
        }
        return true;

    }

    T PopFront() {
        T result{};
        if (_isIterating) {
            log_device(true, __FUNCTION__, "Nao pode modificar a lista enquanto a itera");
            return result;
        }

        if (Lock()) {
            result = _list.front();
            _list.pop_front();
            Unlock();
        }
        return result;
    }

    void Push(T item) {
        if (_isIterating) {
            log_device(true, __FUNCTION__, "Nao pode modificar a lista enquanto a itera");
            return;
        }
        if (Lock()) {
            _list.push_back(item);
            Unlock();
        }
    }

    typename std::list<T>::iterator begin() {
        if (Lock()) {
            _isIterating = true;
            return _list.begin();
        }
        return _list.end();
    }

    typename std::list<T>::iterator end() {
        Unlock();
        _isIterating = false;
        return _list.end();
    }

    bool Remove(T item, std::function<bool(T, T)> compareFunction) {
        if (_isIterating) {
            log_device(true, __FUNCTION__, "Nao pode modificar a lista enquanto a itera");
            return false;
        }
        auto res = false;
        if (Lock()) {
            for (auto it = _list.begin(); it != _list.end(); ++it) {
                if (compareFunction(*it, item)) {
                    _list.erase(it);
                    res = true;
                    break;
                }
            }
            Unlock();
            _isIterating = false;
        }
        return res;
    }

    auto Size() -> uint32_t {
        if (Lock()) {
            uint32_t size = _list.size();
            Unlock();
            return size;
        }
        return 0;
    }

    void Sort(std::function<bool(const T &, const T &)> compare) {
        if (Lock()) {
            _list.sort(compare);
            Unlock();
        }
    }

private:
    bool _isIterating = false;
    std::list<T> _list = std::list<T>();
};


#endif //SAFELIST_H
