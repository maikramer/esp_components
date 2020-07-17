//
// Created by maikeu on 18/08/2019.
//

#ifndef TOMADA_SMART_CONDO_SINGLETON_H
#define TOMADA_SMART_CONDO_SINGLETON_H

#include "esp_log.h"

template<typename T>
class Singleton {
public:

    Singleton(const Singleton &) = delete;

    Singleton &operator=(const Singleton) = delete;

    static T &instance() {
        static T instance{token{}};
        return instance;
    }

protected:
    struct token {
    };

    Singleton() = default;
};

#endif //TOMADA_SMART_CONDO_SINGLETON_H
