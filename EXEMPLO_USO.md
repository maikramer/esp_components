# Exemplo de Uso do Componente

## Adicionando o Componente ao seu Projeto

### Opção 1: Via Git (recomendado para produção)

No arquivo `idf_component.yml` do seu projeto:

```yaml
dependencies:
  esp_components:
    git: https://github.com/seu-usuario/esp_components.git
    version: "^1.0.0"
```

### Opção 2: Via Caminho Local (recomendado para desenvolvimento)

No arquivo `idf_component.yml` do seu projeto:

```yaml
dependencies:
  esp_components:
    path: ../esp_components
```

### Opção 3: Via Registry (se publicado)

```yaml
dependencies:
  esp_components: "^1.0.0"
```

## Usando no seu CMakeLists.txt

### Exemplo 1: Usar todos os componentes

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_components
)
```

### Exemplo 2: Usar componentes específicos

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES Wifi Connection utility ErrorCodes
)
```

### Exemplo 3: Projeto completo

```cmake
cmake_minimum_required(VERSION 3.16)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(meu_projeto)

idf_component_register(
    SRCS 
        "main.cpp"
        "app.cpp"
    INCLUDE_DIRS 
        "."
        "include"
    REQUIRES 
        esp_components
        nvs_flash
        esp_wifi
)
```

## Exemplo de Código C++

```cpp
#include "Wifi.h"
#include "Connection.h"
#include "ErrorCodes.h"
#include "utility/Utility.h"

void app_main() {
    // Usar componentes do esp_components
    // ...
}
```

## Notas Importantes

1. **Submódulos Git**: Se você usar o componente via Git, certifique-se de que os submódulos estão inicializados:
   ```bash
   git submodule update --init --recursive
   ```

2. **Dependências**: O componente requer ESP-IDF v5.0+ e alguns componentes padrão do ESP-IDF.

3. **Componentes Individuais**: Você pode usar apenas os componentes que precisa, não precisa incluir todos.

