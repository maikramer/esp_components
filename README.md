# ESP Components

Biblioteca de componentes reutilizáveis para ESP-IDF, incluindo comunicação (WiFi, Bluetooth), armazenamento, drivers e utilitários.

## Componentes Incluídos

- **BluetoothServer**: Servidor Bluetooth usando NimBLE
- **Connection**: Gerenciamento de conexões base
- **Drivers**: Drivers para bateria e stepper
- **ErrorCodes**: Sistema de códigos de erro
- **IoUtility**: Utilitários de I/O (LED, entrada filtrada)
- **JsonModels**: Modelos JSON e serialização
- **NimBLE**: Wrapper C++ para NimBLE
- **Storage**: Armazenamento (NVS, Flash, SD Card)
- **UserManaging**: Gerenciamento de usuários
- **utility**: Utilitários gerais (eventos, exceções, etc.)
- **Wifi**: Funcionalidades WiFi (servidor, cliente, OTA, Telnet)

## Como Usar

### Adicionar como Componente via idf_component.yml

Adicione este componente ao seu projeto ESP-IDF adicionando ao arquivo `idf_component.yml` na raiz do seu projeto:

```yaml
dependencies:
  esp_components:
    git: https://github.com/maikramer/esp_components.git
    # ou use um caminho local:
    # path: ../esp_components
```

### Usar em seu CMakeLists.txt

Depois de adicionar o componente, você pode usá-lo em seu `CMakeLists.txt`:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_components
)
```

### Usar Componentes Individuais

Quando usar componentes específicos, referencie-os diretamente pelo nome:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES Wifi Connection utility ErrorCodes JsonModels
)
```

**Nota Importante**: Se você encontrar erros de "component not found" ao usar como dependência externa via git, certifique-se de que:
1. Os submódulos Git estão inicializados (execute `git submodule update --init --recursive` no repositório)
2. Você está referenciando os componentes pelo nome correto (ex: `utility`, não `esp_components::utility`)

## Estrutura

```
esp_components/
├── BluetoothServer/    # Servidor Bluetooth
├── Connection/         # Gerenciamento de conexões
├── Drivers/            # Drivers de hardware
├── ErrorCodes/         # Sistema de erros
├── IoUtility/          # Utilitários de I/O
├── JsonModels/         # Modelos JSON
├── NimBLE/             # Wrapper NimBLE C++
├── Storage/            # Armazenamento
├── UserManaging/       # Gerenciamento de usuários
├── utility/            # Utilitários gerais
├── Wifi/               # Funcionalidades WiFi
└── submodules/         # Submódulos Git
```

## Dependências Externas

Este componente requer:
- ESP-IDF v6.0 ou superior
- Componentes ESP-IDF padrão: `esp_wifi`, `esp_netif`, `esp_event`, `nvs_flash`, `bt`, etc.
- Componente externo: `johboh/nlohmann-json` (via IDF Component Manager)

## Licença

MIT

