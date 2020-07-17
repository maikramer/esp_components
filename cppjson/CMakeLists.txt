set(srcs)
set(include_dirs ../submodules/json/single_include/ .)
set(requires)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
target_compile_options(${COMPONENT_LIB} INTERFACE -std=c++17)
