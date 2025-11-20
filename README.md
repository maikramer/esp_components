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
- **Utility**: Utilitários gerais (eventos, exceções, etc.)
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

⚠️ **IMPORTANTE**: Quando o componente é adicionado via git, você deve referenciar os subcomponentes diretamente, não o componente raiz:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES Wifi Connection Utility ErrorCodes JsonModels
)
```

**Não use** `REQUIRES esp_components` quando o componente é baixado via git, pois os subcomponentes podem não ser encontrados automaticamente.

### Usar Componentes Individuais

Quando usar componentes específicos, referencie-os diretamente pelo nome (recomendado):

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES Wifi Connection Utility ErrorCodes JsonModels
)
```

**Nota Importante**: 

⚠️ **Problema conhecido com componentes via Git**: Se você encontrar erros como "Failed to resolve component 'Utility'" ao usar o componente via git, há algumas soluções:

1. **Solução Recomendada**: Use caminho local para desenvolvimento:
   ```yaml
   dependencies:
     esp_components:
       path: ../esp_components
   ```

2. **Alternativa**: Referencie os componentes diretamente no seu `CMakeLists.txt`:
   ```cmake
   REQUIRES Wifi Connection Utility ErrorCodes JsonModels
   ```
   Em vez de apenas `REQUIRES esp_components`

3. **Limpar e reconstruir**: Após mudanças, execute:
   ```bash
   idf.py fullclean
   rm -rf managed_components
   idf.py build
   ```

Veja `TROUBLESHOOTING.md` para mais detalhes sobre problemas comuns.

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
├── Utility/            # Utilitários gerais
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

