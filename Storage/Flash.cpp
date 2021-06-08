//
// Created by maikeu on 07/07/2020.
//

#include "Flash.h"

wl_handle_t Flash::_wearHandle = WL_INVALID_HANDLE;
bool Flash::_init = false;

auto Flash::Init() -> ErrorCode {
    Storage::InitErrors();
    const esp_vfs_fat_mount_config_t mount_config = {
            .format_if_mount_failed = true,
            .max_files = 4,
            .allocation_unit_size = CONFIG_WL_SECTOR_SIZE
    };
    Storage::_sectorSize = CONFIG_WL_SECTOR_SIZE;

    esp_err_t err = esp_vfs_fat_spiflash_mount(StorageConst::BasePath, "storage", &mount_config, &_wearHandle);
    if (err != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Falha montando sistema de arquivos FATFS: (%s)", esp_err_to_name(err));
    } else {
        _init = true;
    }

    return ToMountError(err);
}

auto Flash::ToMountError(esp_err_t error) -> ErrorCode {
    if (error == ESP_OK) {
        return ErrorCode(ErrorCodes::None);
    } else if (error == ESP_ERR_NOT_FOUND) {
        return ErrorCode(ErrorCodes::PartitionNotFound);
    } else if (error == ESP_ERR_NO_MEM) {
        return ErrorCode(ErrorCodes::NoFreeMemory);
    } else if (error == ESP_ERR_INVALID_STATE) {
        return ErrorCode(ErrorCodes::AlreadyMounted);
    }

    return ErrorCode(ErrorCodes::Error);

}
