set(srcs "SdCard.cpp"
        "SdCard_user.cpp"
        "SdCard_config.cpp"
        "Flash.cpp")
set(include_dirs .)
set(requires config utility sdmmc vfs fatfs UserManaging cppjson)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c++17)