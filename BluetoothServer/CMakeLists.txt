set(srcs BluetoothServer.cpp BluetoothManager.cpp Commander.cpp BluetoothConnection.cpp CommandCode.cpp)
set(include_dirs .)
set(requires config NimBLE JsonModels ErrorCodes Connection)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")