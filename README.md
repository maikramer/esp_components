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
    git: https://github.com/seu-usuario/esp_components.git
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

Você também pode usar componentes específicos diretamente:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES Wifi Connection utility
)
```

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
- ESP-IDF v5.0 ou superior
- Componentes ESP-IDF padrão: `esp_wifi`, `nvs_flash`, `bt`, etc.
- Componente externo: `johboh__nlohmann-json` (via IDF Component Manager)

## Licença

MIT

