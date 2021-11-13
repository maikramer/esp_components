//
// Created by maikeu on 18/08/2021.
//

#ifndef PROJETO_ALPHA_LOCKABLECONTAINER_H
#define PROJETO_ALPHA_LOCKABLECONTAINER_H

#ifdef STM32L1

#include <FreeRTOS.h>
#include <semphr.h>

#define MEDIUM_PRIORITY 4
#elif defined(ESP_PLATFORM)

#include <freertos/FreeRTOS.h>
#include <freertos/semphr.h>
#include "CrossPlatformUtility.h"

#endif

class LockableContainer {
public:
    bool IsLocked() const {
        return _isLocked;
    }

protected:
    bool Lock() {
        auto res = xSemaphoreTake(_mutex, 1000) == pdPASS;
        if (res) {
            _isLocked = true;
        } else {
            log_device(true, __FUNCTION__, "Falha Obtendo Trava");
        }

        return (res);
    }

    void Unlock() {
        if (!_isLocked) return;
        xSemaphoreGive(_mutex);
        _isLocked = false;
    }

private:
    SemaphoreHandle_t _mutex = xSemaphoreCreateMutex();
    bool _isLocked = false;

};


#endif //PROJETO_ALPHA_LOCKABLECONTAINER_H
