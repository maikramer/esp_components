//
// Created by maikeu on 23/09/2019.
//
#ifdef USE_SDCARD

#include "SdCard.h"
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include "esp_err.h"
#include "esp_log.h"
#include "esp_vfs_fat.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include <GeneralUtils.h>
#include <projectConfig.h>

#define MOUNT_POINT "/sdcard"
#define SPI_DMA_CHAN    1

auto SdCard::Init() -> bool {

    // Options for mounting the filesystem.
    // If format_if_mount_failed is set to true, SD card will be partitioned and
    // formatted in case when mounting fails.
    esp_vfs_fat_sdmmc_mount_config_t mount_config = {
            .format_if_mount_failed = false,
            .max_files = MAX_OPEN_FILES,
            .allocation_unit_size = 16 * 1024
    };
    sdmmc_card_t *card = nullptr;
    const char mount_point[] = MOUNT_POINT;
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

    ret = esp_vfs_fat_sdspi_mount(mount_point, &host, &slot_config, &mount_config, &card);

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

auto SdCard::FastStoreKeyValue(const std::string &key, const std::string &value, const std::string &fileName) -> bool {
    const char *TAG = __FUNCTION__;

    ESP_LOGI(TAG, "Opening file");
    std::stringstream str{};
    str << "/sdcard/" << fileName << ".txt";
    auto path = str.str();
    std::ofstream ofs(path, std::ofstream::app);
    if (!ofs) {
        ESP_LOGE(TAG, "Erro abrindo ou criando arquivo");
        return false;
    }

    ofs << key << '=' << value << std::endl;
    ofs.close();

    ESP_LOGI(TAG, "File written");

    return true;
}

auto SdCard::StoreKeyValue(const std::string &key, const std::string &value, const std::string &fileName,
                           bool overwrite) -> StoreResult {
    const char *TAG = __FUNCTION__;
    StoreResult result = StoreResult::Ok;

    std::stringstream str{};
    str << "/sdcard/" << fileName << ".txt";
    auto path = str.str();
    ESP_LOGI(TAG, "Opening %s", path.c_str());

    std::ifstream input(path, std::ifstream::in); //File to read from
    if (!input) {
        std::ofstream ofs(path, std::ofstream::app);
        ofs << key << '=' << value << '\n';
        ofs.close();
    } else {
        std::ofstream ofs("/sdcard/temp.txt", std::ofstream::app);
        if (!ofs) {
            ESP_LOGE(__FUNCTION__, "Erro Abrindo arquivos");
            result = StoreResult::Error;
            goto end;
        }

        std::string strTemp{};
        bool found = false;
        while (input >> strTemp) {
            if (!found && strTemp.find(key) != std::string::npos) {
                found = true;
                result = StoreResult::Exist;
                if (overwrite) {
                    ESP_LOGI(__FUNCTION__, "Chave encontrada, atualizando");
                    ofs << key << '=' << value << '\n';
                } else {
                    ofs << strTemp << '\n';
                    ESP_LOGI(__FUNCTION__, "Chave encontrada, mas, nao deve ser atualizada");
                }
            } else {
                ofs << strTemp << '\n';
            }
        }

        if (!found) {
            ofs << key << '=' << value << '\n';
        }

        ofs.close();
        input.close();

        if (found && !overwrite) {
            remove("/sdcard/temp.txt");
            goto end;
        }

        if (remove(path.c_str()) != 0) {
            ESP_LOGE(__FUNCTION__, "Erro Apagando arquivo Antigo");
            result = StoreResult::Error;
            goto end;
        } else {
            ESP_LOGI(__FUNCTION__, "Arquivo Antigo apagado");
        }

        if (rename("/sdcard/temp.txt", path.c_str()) != 0) {
            ESP_LOGE(__FUNCTION__, "Erro Renomeando Temp");
            result = StoreResult::Error;
            goto end;
        } else {
            ESP_LOGI(__FUNCTION__, "Temp renomeado para %s", fileName.c_str());
        }
    }

    ESP_LOGI(TAG, "Arquivo %s salvo", path.c_str());

    end:
    return result;
}

auto SdCard::ReadKeyFromFile(const std::string &key, const std::string &fileName) -> std::string {
    const char *TAG = __FUNCTION__;
    ESP_LOGI(TAG, "xSemaphore took");

    std::string path = "/sdcard/" + fileName;
    ESP_LOGI(TAG, "Opening %s", path.c_str());

    std::ifstream input(path, std::ifstream::in);
    std::string line{};
    std::string res;
    if (!input) {
        ESP_LOGE(__FUNCTION__, "Arquivo %s nao encontrado ou nao criado ainda", path.c_str());
        goto end;
    }

    while (input >> line) {
        if (line.find(key) != std::string::npos) {
            if (line.empty()) {
                ESP_LOGW(__FUNCTION__, "Linha vazia");
                continue;
            }

            auto eq = line.find('=');
            if (eq == std::string::npos) {
                ESP_LOGW(__FUNCTION__, "Igual nao encotrado na linha");
                continue;
            }

            res = line.substr(eq + 1);
            input.close();
            ESP_LOGI(__FUNCTION__, "Lido %s da chave %s", res.c_str(), key.c_str());
            goto end;
        }
    }

    input.close();

    end:
    return res;
}

#endif