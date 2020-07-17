set(srcs "SdCard.cpp"
        "Storage_config.cpp"
        "Flash.cpp"
        "NVS.cpp"
        "Storage.cpp"
        "Storage_user.cpp")
set(include_dirs .)
set(requires config utility sdmmc vfs fatfs UserManaging cppjson nvs_flash)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c++17)