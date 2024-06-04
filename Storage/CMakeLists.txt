set(srcs "SdCard.cpp"
        "Flash.cpp"
        "NVS.cpp"
        "Storage.cpp")
set(include_dirs .)
set(requires config utility sdmmc vfs fatfs UserManaging nvs_flash)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")