//
// Created by maikeu on 07/07/2020.
//
#include <sstream>
#include <dirent.h>
#include "Storage.h"
#include <ff.h>

uint32_t Storage::_sectorSize;//NOLINT

void remove_dir(const char *path) {
    struct dirent *entry = nullptr;
    DIR *dir = nullptr;
    dir = opendir(path);
    while ((entry = readdir(dir))) {//NOLINT
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

bool compare_files(const std::string &p1, const std::string &p2) {
    std::ifstream f1(p1, std::ifstream::binary | std::ifstream::ate);
    std::ifstream f2(p2, std::ifstream::binary | std::ifstream::ate);

    if (f1.fail() || f2.fail()) {
        ESP_LOGE(__FUNCTION__, "Erro na abertura dos arquivos");
        return false; //file problem
    }

    if (f1.tellg() != f2.tellg()) {
        ESP_LOGE(__FUNCTION__, "Tamanhos sao diferentes");
        return false; //size mismatch
    }

    //seek back to beginning and use std::equal to compare contents
    f1.seekg(0, std::ifstream::beg);
    f2.seekg(0, std::ifstream::beg);
    bool equals = std::equal(std::istreambuf_iterator<char>(f1.rdbuf()),
                             std::istreambuf_iterator<char>(),
                             std::istreambuf_iterator<char>(f2.rdbuf()));

    if (!equals) {
        ESP_LOGE(__FUNCTION__, "Tamanhos sao diferentes");
    }

    return equals;
}

auto Storage::EraseData() -> ErrorCode {
    remove_dir(StorageConst::BasePath);
    return ErrorCodes::None;
}

auto Storage::DeleteFile(const std::string &fileName) -> ErrorCode {
    std::stringstream str{};
    str.setf(std::ios::fixed);
    str << StorageConst::BasePath << "/" << fileName << ".txt";
    auto path = str.str();
    auto stream = std::ifstream(path, std::ifstream::in);
    if (stream.fail()) {
        ESP_LOGW(__FUNCTION__, "Tentando apagar arquivo inexistente");
        return ErrorCodes::FileNotFound;
    }
    stream.close();

    std::remove(path.c_str());

    stream = std::ifstream(path);
    if (!stream.fail()) {
        ESP_LOGE(__FUNCTION__, "Erro apagando arquivo");
        stream.close();
        return ErrorCodes::StorageError;
    }

    return ErrorCodes::None;
}

auto Storage::CopyFile(const std::string &fileSource, const std::string &fileDest) -> ErrorCode {
    std::stringstream str{};
    str.setf(std::ios::fixed);
    str << StorageConst::BasePath << "/" << fileSource << ".txt";
    auto pathSource = str.str();
    auto existTest = std::ifstream(pathSource, std::ifstream::in);
    if (existTest.fail()) {
        ESP_LOGW(__FUNCTION__, "Tentando copiar arquivo inexistente");
        return ErrorCodes::FileNotFound;
    }

    str.str("");
    str.clear();
    str << StorageConst::BasePath << "/" << fileDest << ".txt";
    auto pathDest = str.str();

    //Apaga o arquivo anterior
    auto result = DeleteFile(fileDest);
    if (result == ErrorCodes::StorageError) {
        ESP_LOGE(__FUNCTION__, "Erro apagando arquivo");
        return ErrorCodes::StorageError;
    }

    std::ifstream source(pathSource, std::ios::binary);
    std::ofstream dest(pathDest, std::ios::binary & std::ios::app);
    if (source.fail() || dest.fail()) {
        ESP_LOGE(__FUNCTION__, "Erro abrindo os arquivos");
        return ErrorCodes::StorageError;
    }

    std::istreambuf_iterator<char> begin_source(source);
    std::istreambuf_iterator<char> end_source;
    std::ostreambuf_iterator<char> begin_dest(dest);

    //Copia o arquivo, a forma de testar nao garante
    ESP_LOGI(__FUNCTION__, "Copiando de %s para %s", pathSource.c_str(), pathDest.c_str());
    std::copy(begin_source, end_source, begin_dest);
    source.close();
    dest.close();

    bool success = compare_files(pathSource, pathDest);
    if (!success) {
        ESP_LOGE(__FUNCTION__, "Arquivos nao sao iguais");
        return ErrorCodes::StorageError;
    }

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