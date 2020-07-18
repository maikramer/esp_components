//
// Created by maikeu on 23/09/2019.
//
#include <projectConfig.h>

#ifdef USE_SDCARD
#include "SdCard.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#define SPI_DMA_CHAN    1

auto SdCard::Init() -> bool {
    Storage::InitErrors();
    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = MAX_OPEN_FILES,
            .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card = nullptr;
    ESP_LOGI(__FUNCTION__, "Initializing SD card");

    sdmmc_host_t host = SDSPI_HOST_DEFAULT();
    spi_bus_config_t bus_cfg = {
            .mosi_io_num = SDMOSI,
            .miso_io_num = SDMISO,
            .sclk_io_num = SDCLK,
            .quadwp_io_num = -1,
            .quadhd_io_num = -1,
            .max_transfer_sz = 4000,
            .flags = 0,
            .intr_flags=0
    };

    esp_err_t ret = spi_bus_initialize(static_cast<spi_host_device_t>(host.slot), &bus_cfg, SPI_DMA_CHAN);
    if (ret != ESP_OK) {
        ESP_LOGE(__FUNCTION__, "Failed to initialize bus.");
        return false;
    }

    // This initializes the slot without card detect (CD) and write protect (WP) signals.
    // Modify slot_config.gpio_cd and slot_config.gpio_wp if your board has these signals.
    sdspi_device_config_t slot_config = SDSPI_DEVICE_CONFIG_DEFAULT();
    slot_config.gpio_cs = SDCS0;

    ret = esp_vfs_fat_sdspi_mount(StorageConst::BasePath, &host, &slot_config, &mount_config, &card);

    if (ret != ESP_OK) {
        if (ret == ESP_FAIL) {
            ESP_LOGE(__FUNCTION__, "Failed to mount filesystem. "
                                   "If you want the card to be formatted, set format_if_mount_failed = true.");
        } else {
            ESP_LOGE(__FUNCTION__, "Failed to initialize the card (%s). "
                                   "Make sure SD card lines have pull-up resistors in place.", esp_err_to_name(ret));
        }
        return false;
    } else {
        sdmmc_card_print_info(stdout, card);
        return true;
    }

}
#endif