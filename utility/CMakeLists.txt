set(srcs Singleton.cpp Utility.cpp SafeList.cpp)
set(include_dirs .)
set(requires nvs_flash ArduinoBLE)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c++17)
