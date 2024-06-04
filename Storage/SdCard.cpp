#include "projectConfig.h"


#include "SdCard.h"
#ifdef USE_SDCARD
/**
 * @file SdCard.cpp
 * @brief Implementation for the SdCard class for SD card initialization and mounting.
 */

ErrorCode SdCard::initialize(const char* mountPoint, bool formatIfFailed) {
    ESP_LOGI("SdCard", "Initializing SD card...");

    // Configuration for mounting the SD card
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
        .format_if_mount_failed = formatIfFailed,
        .max_files = 5,          
        .allocation_unit_size = 16 * 1024  
    };

    // Use default SPI host and device configurations
    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SDCS0; // Replace with your CS pin if different

    // Initialize SPI bus
    esp_err_t ret = spi_bus_initialize(host.slot, &slot_config, SPI_DMA_CH_AUTO);
    if (ret != ESP_OK) {
        ESP_LOGE("SdCard", "Failed to initialize SPI bus: %s", esp_err_to_name(ret));
        return CommonErrorCodes::StorageInitFailed;
    }

    sdmmc_card_t* card;
    ret = esp_vfs_fat_sdspi_mount(mountPoint, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE("SdCard", "Failed to mount filesystem. "
                     "If you want the card to be formatted, set formatIfFailed = true.");
        } else {
            ESP_LOGE("SdCard", "Failed to initialize the card (%s). "
                     "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return CommonErrorCodes::StorageInitFailed;
    }

    // Card has been initialized
    sdmmc_card_print_info(stdout, card);
    ESP_LOGI("SdCard", "SD card mounted at: %s", mountPoint);
    return CommonErrorCodes::None;
}

#endif // USE_SDCARD