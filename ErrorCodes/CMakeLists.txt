set(srcs ErrorCode.cpp GeneralErrorCodes.cpp NetworkErrorCodes.cpp WifiErrorCodes.cpp
        BluetoothErrorCodes.cpp FileErrorCodes.cpp StorageErrorCodes.cpp UserErrorCodes.cpp
        HardwareErrorCodes.cpp CommunicationErrorCodes.cpp)
set(include_dirs .)
set(requires utility config JsonModels)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")