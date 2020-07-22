//
// Created by maikeu on 07/07/2020.
//
#include <sstream>
#include <dirent.h>
#include "Storage.h"
#include <ff.h>

uint32_t Storage::_sectorSize;

void remove_dir(const char *path) {
    struct dirent *entry = nullptr;
    DIR *dir = nullptr;
    dir = opendir(path);
    while ((entry = readdir(dir))) {
        DIR *sub_dir = nullptr;
        FILE *file = nullptr;
        if (*(entry->d_name) != '.') {
            std::stringstream str{};
            str << path << "/" << entry->d_name;
            if ((sub_dir = opendir(str.str().c_str()))) {
                closedir(sub_dir);
                remove_dir(str.str().c_str());
            } else {
                if ((file = fopen(str.str().c_str(), "r"))) {
                    fclose(file);
                    remove(str.str().c_str());
                }
            }
        }
    }
    if (path != StorageConst::BasePath) {
        remove(path);
    }

}

auto Storage::EraseData() -> ErrorCode {
    remove_dir(StorageConst::BasePath);
    return ErrorCodes::None;
}

auto Storage::GetStorageStatus(StorageStatus &status) -> ErrorCode {
    FATFS *fs = nullptr;
    uint32_t fre_clust = 0;
    uint64_t freeSectors = 0;
    uint64_t totalSectors = 0;

    /* Get volume information and free clusters of drive 0 */
    auto res = f_getfree("0:", &fre_clust, &fs);
    if (res != FR_OK) {
        ESP_LOGE(__FUNCTION__, "Erro obtendo informacoes do disco");
        return ErrorCode(ErrorCodes::Error);
    }
    /* Get total sectors and free sectors */
    totalSectors = (fs->n_fatent - 2) * fs->csize;
    freeSectors = fre_clust * fs->csize;

    status.FreeSpace = freeSectors * _sectorSize;
    status.TotalSpace = totalSectors * _sectorSize;
    ESP_LOGI(__FUNCTION__, "%llu bytes livres do total de %llu", status.FreeSpace, status.TotalSpace);
    return ErrorCode(ErrorCodes::None);
}

bool Storage::CheckFileNameForReserved(const std::string &fileName) {
    if ((std::strcmp(fileName.c_str(), StorageConst::ConfigFilename) == 0) ||
        (std::strcmp(fileName.c_str(), StorageConst::UsersFilename) == 0)) {
        ESP_LOGE(__FUNCTION__, "Arquivo com nome %s é reservado", fileName.c_str());
        return true;
    }
    return false;
}