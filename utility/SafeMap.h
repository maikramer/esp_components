//
// Created by maikeu on 27/07/2021.
//

#ifndef LOCKABLEMAP_H
#define LOCKABLEMAP_H

#include<LockableContainer.h>

template<class TKey, class TValue>
class SafeMap : public LockableContainer{
private:
    std::map<TKey, TValue> _internalMap{};
    std::map<TKey, TValue> _emptyMap{};

public:
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

    bool Remove(TKey key) {
        if (!Lock()) return false;
        bool erased = _internalMap.erase(key);
        Unlock();
        return (erased > 0);
    }
};

#endif //LOCKABLEMAP_H
