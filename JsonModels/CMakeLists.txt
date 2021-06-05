set(srcs "JsonModels.cpp")
set(include_dirs .)
set(requires utility config cppjson ErrorCodes UserManaging)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} PRIVATE -std=c++17)