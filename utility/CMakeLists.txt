set(srcs Utility.cpp CrossPlatformUtility.cpp Event.cpp)
# Removido ../submodules/nameof/include - submódulo não inicializado e não usado diretamente neste componente
# Se nameof for necessário, inicialize o submódulo: git submodule update --init --recursive
set(include_dirs .)
# Removido ErrorCodes da lista de requires - ErrorCodes depende de utility, não o contrário
# Nota: No REQUIRES usamos 'nlohmann-json' (nome do componente), não 'johboh/nlohmann-json' (nome no registry)
set(requires nvs_flash nlohmann-json)

idf_component_register(SRCS "${srcs}" INCLUDE_DIRS "${include_dirs}" REQUIRES "${requires}")
