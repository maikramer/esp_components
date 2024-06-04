#include "Storage.h"
#include <dirent.h>
#include <sys/stat.h>

/**
 * @file Storage.cpp
 * @brief Implementation of the Storage class for managing files and data on external storage.
 */

uint32_t Storage::_sectorSize;

ErrorCode Storage::initialize() {
    // Create the base storage directory if it doesn't exist
    struct stat st = {0};
    if (stat(StorageConstants::BasePath, &st) == -1) {
        if (mkdir(StorageConstants::BasePath, 0755) != 0) {
            ESP_LOGE("Storage", "Failed to create base storage directory: %s", StorageConstants::BasePath);
            return CommonErrorCodes::OperationFailed;
        }
    }

    // You may need to add additional initialization code here, such as:
    // - Mounting the SD card if necessary
    // - Initializing other storage devices

    // Get sector size for storage status
    FATFS *fs = nullptr;
    if (f_getfree("0:", nullptr, &fs) != FR_OK) {
        ESP_LOGE("Storage", "Failed to get storage information.");
        return CommonErrorCodes::StorageInitFailed;
    }

    if (fs != nullptr) {
        _sectorSize = fs->csize * 512; // Sector size in bytes
    } else {
        ESP_LOGE("Storage", "Failed to get storage sector size.");
        return CommonErrorCodes::StorageInitFailed;
    }

    return CommonErrorCodes::None;
}

ErrorCode Storage::eraseData() {
    DIR *dir = opendir(StorageConstants::BasePath);
    if (dir == nullptr) {
        ESP_LOGE("Storage", "Failed to open storage directory: %s", StorageConstants::BasePath);
        return CommonErrorCodes::OperationFailed;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue; // Skip "." and ".." entries
        }

        std::string filePath = getFilePath(entry->d_name);
        if (remove(filePath.c_str()) != 0) {
            ESP_LOGE("Storage", "Failed to delete file: %s", filePath.c_str());
            closedir(dir);
            return CommonErrorCodes::OperationFailed;
        }
    }

    closedir(dir);
    return CommonErrorCodes::None;
}

ErrorCode Storage::deleteFile(const std::string& fileName) {
    std::string filePath = getFilePath(fileName);
    if (remove(filePath.c_str()) != 0) {
        ESP_LOGE("Storage", "Failed to delete file: %s", filePath.c_str());
        return CommonErrorCodes::FileNotFound;
    }
    return CommonErrorCodes::None;
}

ErrorCode Storage::copyFile(const std::string& sourceFileName, const std::string& destinationFileName) {
    std::string sourcePath = getFilePath(sourceFileName);
    std::string destPath = getFilePath(destinationFileName);

    std::ifstream sourceFile(sourcePath, std::ios::binary);
    std::ofstream destFile(destPath, std::ios::binary);

    if (!sourceFile.is_open()) {
        ESP_LOGE("Storage", "Failed to open source file for copying: %s", sourcePath.c_str());
        return CommonErrorCodes::FileOpenError;
    }
    if (!destFile.is_open()) {
        ESP_LOGE("Storage", "Failed to open destination file for copying: %s", destPath.c_str());
        sourceFile.close();
        return CommonErrorCodes::FileOpenError;
    }

    destFile << sourceFile.rdbuf();

    sourceFile.close();
    destFile.close();

    return CommonErrorCodes::None;
}

ErrorCode Storage::getStatus(StorageStatus& status) {
    FATFS *fs = nullptr;
    DWORD fre_clust;

    if (f_getfree("0:", &fre_clust, &fs) != FR_OK) {
        ESP_LOGE("Storage", "Failed to get storage status.");
        return CommonErrorCodes::StorageInitFailed;
    }

    if (fs == nullptr) {
        return CommonErrorCodes::StorageInitFailed;
    }

    status.totalSpace = (fs->n_fatent - 2) * fs->csize * _sectorSize;
    status.freeSpace = fre_clust * fs->csize * _sectorSize;

    return CommonErrorCodes::None;
}

#ifdef USER_MANAGEMENT_ENABLED
// User-related storage operations
ErrorCode Storage::getEntriesFromUser(const std::string& user, std::map<int64_t, uint32_t>& dataMap) {
    return getEntriesFromFile(user, dataMap);
}

ErrorCode Storage::storeUser(const JsonModels::User& user, bool overwrite) {
    ESP_LOGI("Storage", "Storing user: %s", user.Name.c_str());
    if (isReservedFileName(user.Name)) {
        return CommonErrorCodes::ArgumentError;
    }
    return storeKeyValueInternal(user.Name, user, StorageConstants::UsersFilename, overwrite);
}

ErrorCode Storage::loadUser(const std::string& userName, JsonModels::User& user) {
    return readKeyValue(userName, user, StorageConstants::UsersFilename);
}

ErrorCode Storage::getAllUsers(std::map<std::string, JsonModels::User>& usersMap) {
    ErrorCode result = getEntriesFromFile(StorageConstants::UsersFilename, usersMap);
    if (result == CommonErrorCodes::FileNotFound || result == CommonErrorCodes::FileIsEmpty) {
        return CommonErrorCodes::UserNotFound;
    }
    return result;
}
#endif

// Configuration storage operations
ErrorCode Storage::storeConfig(const std::string& key, const std::string& value, bool overwrite) {
    ESP_LOGI("Storage", "Storing config: %s", key.c_str());
    return storeKeyValue(key, value, StorageConstants::ConfigFilename, overwrite);
}

ErrorCode Storage::loadConfig(const std::string& key, std::string& value) {
    return readKeyValue(key, value, StorageConstants::ConfigFilename);
}

bool Storage::isReservedFileName(const std::string& fileName) {
    return (fileName == StorageConstants::ConfigFilename ||
            fileName == StorageConstants::UsersFilename ||
            fileName == StorageConstants::InfoFilename);
}

std::string Storage::getFilePath(const std::string& fileName) {
    return std::string(StorageConstants::BasePath) + "/" + fileName + ".txt";
}