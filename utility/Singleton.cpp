//
// Created by maikeu on 18/08/2019.
//

#include <cstdio>
#include "Singleton.h"

Singleton::Singleton() {
    _instance = this;
}

Singleton *Singleton::_instance = nullptr;

Singleton *Singleton::GetInstance() {
    if (_instance == nullptr) {
        printf("\nSingleton nao existe ou nao foi inicializado\n");
        return nullptr;
    }

    return _instance;
}