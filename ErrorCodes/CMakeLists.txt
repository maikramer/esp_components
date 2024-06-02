set(srcs ErrorCode.cpp CommonErrorCodes.cpp)
set(include_dirs .)
set(requires utility config JsonModels)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")