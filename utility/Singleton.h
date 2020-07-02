//
// Created by maikeu on 18/08/2019.
//

#ifndef TOMADA_SMART_CONDO_SINGLETON_H
#define TOMADA_SMART_CONDO_SINGLETON_H


class Singleton {
private:
    /* Here will be the _instance stored. */
    static Singleton *_instance;

    /* Private constructor to prevent instancing. */
    Singleton();

public:
    /* Static access method. */
    static Singleton *GetInstance();
};

/* Null, because _instance will be initialized on demand. */



#endif //TOMADA_SMART_CONDO_SINGLETON_H
