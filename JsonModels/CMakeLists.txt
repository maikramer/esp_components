set(srcs "JsonModels.cpp")
set(include_dirs .)
set(requires utility config cppjson ErrorCodes UserManaging)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")