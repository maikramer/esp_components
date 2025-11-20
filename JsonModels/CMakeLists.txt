set(srcs "JsonModels.cpp")
set(include_dirs .)
# Removido UserManaging da lista de requires para evitar dependência circular
# JsonModels pode funcionar sem UserManaging, UserManaging é opcional
set(requires utility config johboh/nlohmann-json ErrorCodes)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")