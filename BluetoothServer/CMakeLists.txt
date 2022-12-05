set(srcs BluetoothServer.cpp BluetoothManager.cpp Commander.cpp BluetoothConnection.cpp ConnectionManager.cpp)
set(include_dirs .)
set(requires config ArduinoNimBLE JsonModels ErrorCodes)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")