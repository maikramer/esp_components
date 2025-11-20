# Troubleshooting - Problemas Comuns

## Erro: "Failed to resolve component 'utility' required by component 'JsonModels': unknown name"

Este erro ocorre quando o componente é adicionado via git e os subcomponentes não são detectados automaticamente.

### Solução 1: Usar caminho local (Recomendado para desenvolvimento)

Em vez de usar git, use um caminho local no `idf_component.yml` do seu projeto:

```yaml
dependencies:
  esp_components:
    path: ../esp_components
```

### Solução 2: Verificar estrutura do componente

Quando o componente é baixado via git, ele é colocado em `managed_components/esp_components/`. O ESP-IDF deve detectar automaticamente os subcomponentes em subdiretórios que contenham `CMakeLists.txt`.

Certifique-se de que:
1. Não há um `CMakeLists.txt` na raiz do componente (foi removido para permitir detecção automática)
2. Cada subcomponente tem seu próprio `CMakeLists.txt` em seu diretório
3. Os submódulos Git estão inicializados (se necessário)

### Solução 3: Referenciar componentes diretamente

Se o problema persistir, você pode referenciar os componentes diretamente no seu `CMakeLists.txt`:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES Wifi Connection utility ErrorCodes JsonModels
)
```

Em vez de:

```cmake
idf_component_register(
    SRCS "main.cpp"
    INCLUDE_DIRS "."
    REQUIRES esp_components
)
```

### Solução 4: Limpar e reconstruir

Se o problema persistir após as correções acima:

```bash
# Limpar build
idf.py fullclean

# Remover managed_components
rm -rf managed_components

# Reconstruir
idf.py build
```

## Estrutura Esperada

Quando o componente é baixado via git, a estrutura em `managed_components/esp_components/` deve ser:

```
managed_components/esp_components/
├── BluetoothServer/
│   └── CMakeLists.txt
├── Connection/
│   └── CMakeLists.txt
├── utility/
│   └── CMakeLists.txt
├── Wifi/
│   └── CMakeLists.txt
├── ... (outros componentes)
└── idf_component.yml
```

**Importante**: Não deve haver um `CMakeLists.txt` na raiz de `managed_components/esp_components/` para que os subcomponentes sejam detectados automaticamente.

