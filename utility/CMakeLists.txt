set(srcs Utility.cpp CrossPlatformUtility.cpp Event.cpp)
set(include_dirs . ../submodules/nameof/include)
# Removido ErrorCodes da lista de requires - ErrorCodes depende de utility, não o contrário
set(requires nvs_flash johboh/nlohmann-json)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
