set(srcs BaseConnection.cpp CommandCode.cpp Commander.cpp ConnectionManager.cpp)
set(include_dirs .)
set(requires Utility config JsonModels)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")