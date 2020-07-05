//
// Created by maikeu on 04/07/2020.
//

#ifndef ROCKET_TESTER_EVENT_H
#define ROCKET_TESTER_EVENT_H

#include <functional>
#include "list"
#include "map"

//C - Caller e A o argumento
template<class C, class A>
class Event {
private:
    int counter = 0;
    std::map<int, std::function<void(C, A)>> listeners{};
public:
    // Retorna um handler para remover da lista depois
    int AddListener(std::function<void(C, A)> func);

    void RemoveListener(int handle);

    void FireEvent(C caller, A eventArgs);
};

template<class C, class A>
int Event<C, A>::AddListener(std::function<void(C, A)> func) {
    int handler = counter;
    listeners.insert(std::pair<int, std::function<void(C, A)>>(handler, func));
    counter++;
    return handler;
}

template<class C, class A>
void Event<C, A>::RemoveListener(int handle) {
    listeners.erase(handle);
}

template<class C, class A>
void Event<C, A>::FireEvent(C caller, A eventArgs) {
    for (auto const& f : listeners) {
        f.second(caller, eventArgs);
    }
}




#endif //ROCKET_TESTER_EVENT_H
