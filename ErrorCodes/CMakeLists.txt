set(srcs "ErrorCode.cpp")
set(include_dirs .)
set(requires utility config cppjson JsonModels)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")