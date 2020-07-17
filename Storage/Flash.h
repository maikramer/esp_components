//
// Created by maikeu on 07/07/2020.
//

#ifndef ROCKET_TESTER_FLASH_H
#define ROCKET_TESTER_FLASH_H

#include <esp_vfs_fat.h>
#include "Storage.h"
#include "ErrorCode.h"

class Flash {

public:

    static auto Init() -> ErrorCode;

    static auto ToMountError(esp_err_t error) -> ErrorCode;

private:
    static wl_handle_t _wearHandle;
};


#endif //ROCKET_TESTER_FLASH_H
