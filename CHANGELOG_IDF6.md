# Changelog - Atualização para ESP-IDF v6.0

## Mudanças Realizadas

### Componente Wifi

#### WifiConnection.cpp
- **IP_EVENT_STA_GOT_IP** → **IP_EVENT_STA_GOT_IPV4**: Atualizado o evento de IP para a nova API do IDF v6.0
- **ip_event_got_ip_t** → **ip_event_got_ipv4_t**: Atualizado o tipo do evento
- Adicionado include de `esp_netif.h` no header

#### WifiOta.cpp
- **esp_https_ota()** → **esp_https_ota_begin()**, **esp_https_ota_perform()**, **esp_https_ota_finish()**: Migrado para a nova API assíncrona do IDF v6.0
- A API agora usa um handle (`esp_https_ota_handle_t`) e requer chamadas separadas para begin, perform e finish
- Adicionado tratamento adequado de erros e abort em caso de falha

#### WifiServer.cpp
- Adicionada inicialização de `esp_netif_init()` e `esp_event_loop_create_default()` se ainda não inicializados
- Adicionado include de `esp_event.h`

#### CMakeLists.txt (Wifi)
- Adicionadas dependências explícitas: `esp_netif` e `esp_event`
- Corrigida duplicação de arquivos na lista de SRCS

### Componente BluetoothServer

#### BluetoothServer.cpp
- **vTaskDelayUntil()** → **xTaskDelayUntil()**: Atualizado para a nova API do FreeRTOS no IDF v6.0

### Configuração

#### idf_component.yml
- Adicionado requisito de versão mínima: `idf.version: ">=6.0.0"`
- Especifica que o componente requer ESP-IDF v6.0 ou superior

#### README.md
- Atualizado para refletir que requer ESP-IDF v6.0 ou superior
- Adicionados componentes `esp_netif` e `esp_event` na lista de dependências

### Componente Drivers

#### Stepper.cpp
- **xQueueSemaphoreTake()** → **xSemaphoreTake()**: Substituído pela API correta do FreeRTOS no IDF v6.0

#### Stepper.h
- Removido include de `rom/ets_sys.h` (deprecado)
- Adicionado include de `driver/gpio.h` para compatibilidade

### Componente Utility

#### Utility.cpp
- **GPIO_REG_READ()** → **gpio_get_level()**: Substituído acesso direto ao registro por API oficial
- Removidos includes de `soc/gpio_reg.h` e `rom/gpio.h` (deprecados)
- Adicionado include de `driver/gpio.h`

## APIs Deprecadas Removidas/Substituídas

1. **IP_EVENT_STA_GOT_IP** → **IP_EVENT_STA_GOT_IPV4**
2. **ip_event_got_ip_t** → **ip_event_got_ipv4_t**
3. **esp_https_ota()** → **esp_https_ota_begin()/perform()/finish()**
4. **vTaskDelayUntil()** → **xTaskDelayUntil()**
5. **xQueueSemaphoreTake()** → **xSemaphoreTake()**
6. **GPIO_REG_READ()** → **gpio_get_level()**
7. **rom/ets_sys.h**, **soc/gpio_reg.h**, **rom/gpio.h** → **driver/gpio.h**

## Notas Importantes

- Todas as mudanças são compatíveis apenas com ESP-IDF v6.0+
- O componente agora requer explicitamente ESP-IDF v6.0 ou superior
- As APIs antigas do IDF v4/v5 não são mais suportadas

## Testes Recomendados

Após atualizar, recomenda-se testar:
1. Conexão WiFi em modo STA
2. Servidor WiFi em modo AP
3. Atualizações OTA via HTTPS
4. Funcionalidades Bluetooth

