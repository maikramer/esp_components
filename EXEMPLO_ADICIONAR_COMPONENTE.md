# Como Adicionar Componentes Individuais

## Exemplo: Adicionar o Componente Wifi

### Passo 1: Editar o `idf_component.yml`

No arquivo `idf_component.yml` do seu projeto (geralmente em `main/idf_component.yml` ou na raiz do projeto), adicione:

```yaml
dependencies:
  # Outras dependências do seu projeto...
  lvgl/lvgl: ^9.4.0
  johboh/nlohmann-json: ^3.12.0
  
  # Adicionar o componente Wifi
  Wifi:
    path: ../../esp_components/Wifi
```

**Importante**: O nome `Wifi` deve corresponder ao nome do diretório do componente (`Wifi/`).

### Passo 2: Ajustar o Caminho Relativo

O caminho `../../esp_components/Wifi` assume a seguinte estrutura:

```
seu-projeto/
├── main/
│   └── idf_component.yml  ← Você está aqui
└── esp_components/        ← Componente está aqui
    └── Wifi/
```

Se a estrutura for diferente, ajuste o caminho:
- Se `esp_components` está na mesma pasta do projeto: `path: ../esp_components/Wifi`
- Se `esp_components` está em um nível acima: `path: ../../esp_components/Wifi`
- Se `esp_components` está em outro local: use caminho absoluto ou ajuste relativo

### Passo 3: Usar no CMakeLists.txt

No seu `CMakeLists.txt` (geralmente em `main/CMakeLists.txt`), adicione `Wifi` na lista de `REQUIRES`:

```cmake
idf_component_register(
    SRCS 
        "main.cpp"
        "app.cpp"
    INCLUDE_DIRS "."
    REQUIRES 
        Wifi
        nvs_flash
        esp_wifi
        # outras dependências...
)
```

### Passo 4: Reconfigurar o Projeto

Execute:

```bash
idf.py reconfigure
```

Ou simplesmente:

```bash
idf.py build
```

O Component Manager resolverá automaticamente todas as dependências do Wifi:
- Wifi → Connection, Utility, ErrorCodes
- Connection → Utility, JsonModels
- JsonModels → Utility, ErrorCodes
- ErrorCodes → Utility

## Exemplos de Outros Componentes

### Adicionar apenas Utility

```yaml
dependencies:
  Utility:
    path: ../../esp_components/Utility
```

### Adicionar ErrorCodes

```yaml
dependencies:
  ErrorCodes:
    path: ../../esp_components/ErrorCodes
```

### Adicionar múltiplos componentes

```yaml
dependencies:
  Wifi:
    path: ../../esp_components/Wifi
  Drivers:
    path: ../../esp_components/Drivers
  Storage:
    path: ../../esp_components/Storage
```

### Adicionar BluetoothServer

```yaml
dependencies:
  BluetoothServer:
    path: ../../esp_components/BluetoothServer
```

## Verificação

Após adicionar o componente, você pode verificar se foi encontrado:

```bash
idf.py show-property-value COMPONENT_DIRS | grep Wifi
```

Ou simplesmente tente compilar:

```bash
idf.py build
```

Se houver erros de dependências não encontradas, verifique:
1. O caminho está correto?
2. O nome do componente corresponde ao diretório?
3. O componente tem um `idf_component.yml` válido?

## Estrutura de Diretórios Recomendada

Para facilitar o uso, recomenda-se esta estrutura:

```
projetos/
├── seu-projeto/
│   ├── main/
│   │   ├── idf_component.yml
│   │   ├── CMakeLists.txt
│   │   └── main.cpp
│   └── CMakeLists.txt
└── esp_components/  ← Componentes compartilhados
    ├── Wifi/
    ├── Utility/
    ├── ErrorCodes/
    └── ...
```

Com essa estrutura, use `path: ../../esp_components/Wifi` no `idf_component.yml`.
