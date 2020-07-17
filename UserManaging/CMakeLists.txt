set(srcs UsageData.cpp UserManager.cpp UserManager_Login.cpp ConnectedUser.cpp)
set(include_dirs .)
set(requires Storage BluetoothServer JsonModels)

idf_component_register(SRCS "UsageData.cpp" "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c++17)