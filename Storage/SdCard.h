//
// Created by maikeu on 23/09/2019.
//

#ifndef SDCARD_H
#define SDCARD_H

#include "projectConfig.h"

#ifdef USE_SDCARD
#include <Storage.h>
#include <custom/FreeRTOS.h>
#include <freertos/semphr.h>
#include <map>
#include <esp_log.h>

class SdCard {
public:

    static auto Init() -> bool;
};
#endif

#endif //SDCARD_H
