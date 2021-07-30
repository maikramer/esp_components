//
// Created by maikeu on 27/07/2021.
//

#ifndef LOCKABLEMAP_H
#define LOCKABLEMAP_H

template<class TKey, class TValue>
class SafeMap {
    std::map<TKey, TValue> _internalMap{};
    std::map<TKey, TValue> _emptyMap{};
    SemaphoreHandle_t _mutex = xSemaphoreCreateMutex();
    bool _isLocked = false;
public:
    bool Lock() {
        auto res = xSemaphoreTake(_mutex, 1000) == pdPASS;
        if (res) {
            _isLocked = true;
        } else {
            ESP_LOGE(__FUNCTION__, "Falha Obtendo Trava");
        }

        return (res);
    }

    void Unlock() {
        if (!_isLocked) return;
        xSemaphoreGive(_mutex);
    }

    bool HasKey(TKey key) {
        if (!Lock()) return false;
        auto res = _internalMap.find(key);
        Unlock();
        return (res != _internalMap.end());
    }

    std::tuple<bool, TValue> operator[](TKey key) {
        TValue value;
        bool isValid = false;
        auto hasKey = HasKey(key);
        if (!Lock()) return {false, value};
        if (hasKey) {
            value = _internalMap[key];
            isValid = true;
        }
        Unlock();
        return {isValid, value};

    }

    std::map<TKey, TValue> &StartIteration() {
        if (!Lock()) return _emptyMap;
        return _internalMap;
    }

    void StopIteration() {
        Unlock();
    }

    bool AddOrUpdate(TKey key, TValue value) {
        if (!Lock()) return false;
        _internalMap[key] = value;
        Unlock();
        return true;
    }
};

#endif //LOCKABLEMAP_H
