# Componentes Individuais - Guia de Uso

Este repositório foi reorganizado para permitir que você adicione apenas os componentes que precisa, um a um, via path no `idf_component.yml`.

## Estrutura de Dependências

```
utility (base)
├── ErrorCodes
│   └── JsonModels
│       ├── Connection
│       ├── Storage
│       │   └── UserManaging
│       ├── BluetoothServer (também requer NimBLE e Connection)
│       ├── Wifi (também requer Connection)
│       ├── Drivers
│       └── IoUtility
└── NimBLE
    └── BluetoothServer
```

## Como Usar Componentes Individuais

### Exemplo 1: Usar apenas utility e ErrorCodes

No seu `idf_component.yml`:

```yaml
dependencies:
  utility:
    path: ../esp_components/utility
  ErrorCodes:
    path: ../esp_components/ErrorCodes
```

No seu `CMakeLists.txt`:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES utility ErrorCodes
)
```

### Exemplo 2: Usar Wifi (com todas as dependências automáticas)

No seu `idf_component.yml`:

```yaml
dependencies:
  Wifi:
    path: ../esp_components/Wifi
```

O Component Manager resolverá automaticamente as dependências:
- Wifi → Connection, utility, ErrorCodes
- Connection → utility, JsonModels
- JsonModels → utility, ErrorCodes
- ErrorCodes → utility

### Exemplo 3: Usar BluetoothServer

No seu `idf_component.yml`:

```yaml
dependencies:
  BluetoothServer:
    path: ../esp_components/BluetoothServer
```

Dependências resolvidas automaticamente:
- BluetoothServer → NimBLE, JsonModels, ErrorCodes, Connection
- NimBLE → utility
- JsonModels → utility, ErrorCodes
- ErrorCodes → utility
- Connection → utility, JsonModels

### Exemplo 4: Usar Storage e UserManaging

No seu `idf_component.yml`:

```yaml
dependencies:
  Storage:
    path: ../esp_components/Storage
  UserManaging:
    path: ../esp_components/UserManaging
```

## Componentes Disponíveis

### Componentes Base (sem dependências locais)

- **utility**: Utilitários gerais (eventos, exceções, containers seguros)
  - Dependências externas: `johboh/nlohmann-json`

### Componentes de Nível 1

- **ErrorCodes**: Sistema de códigos de erro padronizado
  - Dependências: `utility`

### Componentes de Nível 2

- **JsonModels**: Modelos JSON e serialização
  - Dependências: `utility`, `ErrorCodes`, `johboh/nlohmann-json`
- **NimBLE**: Wrapper C++ para NimBLE
  - Dependências: `utility`

### Componentes de Nível 3

- **Connection**: Gerenciamento de conexões base
  - Dependências: `utility`, `JsonModels`
- **Storage**: Armazenamento (NVS, Flash, SD Card)
  - Dependências: `utility`, `JsonModels`

### Componentes de Nível 4

- **BluetoothServer**: Servidor Bluetooth usando NimBLE
  - Dependências: `NimBLE`, `JsonModels`, `ErrorCodes`, `Connection`
- **Wifi**: Funcionalidades WiFi
  - Dependências: `utility`, `Connection`, `ErrorCodes`
- **Drivers**: Drivers para bateria e stepper
  - Dependências: `JsonModels`, `ErrorCodes`, `utility`
- **IoUtility**: Utilitários de I/O
  - Dependências: `JsonModels`, `ErrorCodes`
- **UserManaging**: Gerenciamento de usuários
  - Dependências: `Storage`, `JsonModels`, `BluetoothServer`

## Resolução de Dependências Circulares

As seguintes dependências circulares foram resolvidas:

1. **ErrorCodes ↔ JsonModels**: ErrorCodes não requer mais JsonModels diretamente
2. **Storage ↔ UserManaging**: Storage não requer mais UserManaging diretamente
3. **JsonModels ↔ UserManaging**: JsonModels não requer mais UserManaging diretamente

Essas dependências são opcionais e podem ser adicionadas manualmente se necessário.

## Caminhos Relativos

Todos os componentes usam caminhos relativos (`../`) para referenciar outros componentes. Isso significa que:

1. Todos os componentes devem estar no mesmo nível hierárquico
2. O caminho base deve apontar para o diretório `esp_components`
3. Cada componente pode ser adicionado independentemente

## Exemplo Completo

```yaml
## idf_component.yml do seu projeto
dependencies:
  # Adicione apenas os componentes que você precisa
  Wifi:
    path: ../esp_components/Wifi
  Drivers:
    path: ../esp_components/Drivers
```

O Component Manager resolverá automaticamente todas as dependências transitivas!
