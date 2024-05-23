set(srcs FilteredInput.cpp LedBlinker.cpp)
set(include_dirs .)
set(requires JsonModels ErrorCodes)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
