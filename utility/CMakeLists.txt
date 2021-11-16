set(srcs Utility.cpp CrossPlatformUtility.cpp FilteredInput.cpp)
set(include_dirs . ../submodules/nameof/include)
set(requires nvs_flash JsonModels ErrorCodes)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c++17)
