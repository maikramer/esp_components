set(srcs BluetoothServer.cpp Commander.cpp BluetoothConnection.cpp ConnectionManager.cpp)
set(include_dirs .)
set(requires config ArduinoBLE UserManaging JsonModels ErrorCodes)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c++17)