# Documentação Completa dos Módulos

Este documento fornece uma descrição detalhada de todos os módulos disponíveis na biblioteca `esp_components`.

## Índice

1. [BluetoothServer](#bluetoothserver)
2. [Connection](#connection)
3. [Drivers](#drivers)
4. [ErrorCodes](#errorcodes)
5. [IoUtility](#ioutility)
6. [JsonModels](#jsonmodels)
7. [NimBLE](#nimble)
8. [Storage](#storage)
9. [UserManaging](#usermanaging)
10. [Utility](#Utility)
11. [Wifi](#wifi)

---

## BluetoothServer

### Descrição
Módulo que implementa um servidor Bluetooth Low Energy (BLE) usando a biblioteca NimBLE. Fornece funcionalidades completas para criar e gerenciar conexões BLE, incluindo serviços públicos e privados, características de escrita e notificação.

### Classes Principais

#### `BluetoothServer`
Classe singleton que gerencia o servidor BLE principal.

**Métodos principais:**
- `setup(const std::string& deviceName)`: Configura o servidor BLE com um nome de dispositivo
- `createPrivateService()`: Cria um novo serviço privado com UUID único
- `createWriteCharacteristic()`: Cria uma característica de escrita
- `createNotifyCharacteristic()`: Cria uma característica de notificação
- `getPrivateServiceUUID()`: Retorna o UUID do serviço privado

**Características:**
- Singleton pattern para garantir uma única instância
- Suporte a serviços públicos e privados
- Callbacks para eventos de conexão/desconexão
- Envio automático de dados JSON via notificações

#### `BluetoothManager`
Classe singleton que gerencia a funcionalidade BLE do dispositivo.

**Métodos principais:**
- `initialize()`: Inicializa a pilha BLE e configura o servidor
- `createConnection()`: Cria uma nova conexão Bluetooth e adiciona ao ConnectionManager
- `getPrivateServiceUUID()`: Retorna o UUID do serviço privado

**Eventos:**
- `onConnection`: Disparado quando uma nova conexão BLE é estabelecida

#### `BluetoothConnection`
Classe que representa uma conexão Bluetooth individual, herdando de `BaseConnection`.

**Métodos principais:**
- `initialize()`: Inicializa a conexão criando características necessárias
- `connect(uint16_t connId)`: Conecta a conexão a um ID específico
- `disconnect()`: Desconecta e libera recursos
- `sendData()`: Envia dados via notificação ou indicação
- `isFree()`: Verifica se a conexão está livre
- `getId()`: Retorna o ID da conexão
- `getWriteUUID()`: Retorna o UUID da característica de escrita
- `getNotifyUUID()`: Retorna o UUID da característica de notificação

**Características:**
- Herda de `BaseConnection` para interface consistente
- Suporte a gerenciamento de usuários (opcional, via `USER_MANAGEMENT_ENABLED`)
- Thread-safe com mutex para operações de envio
- Callbacks para eventos de escrita e status

#### `BluetoothUtility`
Classe utilitária para operações Bluetooth.

**Métodos:**
- `generateUniqueId(bool isCharacteristic)`: Gera UUIDs únicos para serviços ou características

### Dependências
- `NimBLE`: Wrapper C++ para NimBLE
- `Connection`: Gerenciamento de conexões base
- `JsonModels`: Modelos JSON
- `ErrorCodes`: Sistema de códigos de erro
- `Utility`: Utilitários gerais

### Exemplo de Uso

```cpp
// Inicializar o servidor Bluetooth
ErrorCode err = BluetoothServer::getInstance().setup("MeuDispositivo");
if (err != CommonErrorCodes::None) {
    ESP_LOGE("APP", "Falha ao configurar Bluetooth");
    return;
}

// Criar uma conexão
BluetoothConnection* conn = BluetoothManager::getInstance().createConnection();
conn->initialize();
conn->connect(1);

// Enviar dados
std::vector<uint8_t> data = {1, 2, 3, 4};
conn->sendData(data, true); // true = notificação
```

---

## Connection

### Descrição
Módulo base para gerenciamento de conexões de comunicação. Fornece uma interface abstrata que pode ser implementada por diferentes tipos de conexão (WiFi, Bluetooth, etc.).

### Classes Principais

#### `BaseConnection`
Classe abstrata base para todas as conexões de comunicação.

**Métodos virtuais puros:**
- `disconnect()`: Desconecta a conexão
- `isConnected()`: Verifica se está conectado
- `sendRawData()`: Envia dados brutos

**Métodos implementados:**
- `sendMessage()`: Envia uma string
- `sendJson()`: Envia uma string JSON
- `sendError()`: Envia um erro como JSON (template)
- `sendList()`: Envia uma lista de dados como JSON (template)
- `getNotificationNeeds()`: Retorna o estado de necessidade de notificação
- `setNotificationNeeds()`: Define o estado de necessidade de notificação

**Enum `NotificationNeeds`:**
- `NoSend`: Não precisa enviar notificação
- `SendNormal`: Precisa enviar notificação normal
- `SendImportant`: Precisa enviar notificação importante

**Eventos:**
- `onDisconnect`: Disparado quando a conexão é desconectada

#### `Commander`
Classe estática para processamento de comandos recebidos via conexão.

**Métodos principais:**
- `Init()`: Inicializa o sistema de comandos
- `AddCommand()`: Adiciona um novo comando ao sistema
- `CheckForCommand()`: Verifica e processa comandos em uma string recebida

**Classe `DeviceCommand`:**
- `DataSize`: Tamanho esperado dos dados
- `InternalName`: Nome interno do comando
- `Code`: Código do comando
- `Function`: Função callback a ser executada

#### `ConnectionManager`
Classe estática para gerenciar um pool de conexões.

**Métodos principais:**
- `initialize(size_t numConnections)`: Inicializa o gerenciador com um número de conexões
- `addConnection()`: Adiciona uma conexão ao pool
- `getFreeConnection()`: Obtém uma conexão livre do pool
- `connect(uint16_t id)`: Conecta uma conexão a um ID específico
- `disconnect(uint16_t id)`: Desconecta uma conexão por ID
- `getConnectionById()`: Obtém uma conexão por ID
- `sendNotifications()`: Envia notificações para todas as conexões conectadas
- `notifyAll()`: Define o estado de notificação para todas as conexões

**Eventos:**
- `onConnect`: Disparado quando uma nova conexão é estabelecida

### Dependências
- `Utility`: Utilitários gerais (Event, SafeList)
- `JsonModels`: Modelos JSON
- `ErrorCodes`: Sistema de códigos de erro

### Exemplo de Uso

```cpp
// Inicializar o gerenciador de conexões
ConnectionManager::initialize(5);

// Adicionar uma conexão
BluetoothConnection* conn = new BluetoothConnection();
ConnectionManager::addConnection(conn);

// Conectar a um ID
ErrorCode err = ConnectionManager::connect(1);

// Enviar notificações
ConnectionManager::sendNotifications();
```

---

## Drivers

### Descrição
Módulo contendo drivers para componentes de hardware específicos, incluindo motor de passo (stepper) e monitoramento de bateria.

### Classes Principais

#### `Stepper`
Classe para controlar motores de passo.

**Métodos principais:**
- `Init()`: Inicializa os pinos GPIO do motor
- `SetDirection(bool front)`: Define a direção do movimento
- `Move(int32_t steps, uint32_t speed)`: Move o motor um número de passos
- `Stop()`: Para o movimento do motor

**Propriedades públicas:**
- `Step`: GPIO do pino de passo
- `Direction`: GPIO do pino de direção
- `Enable`: GPIO do pino de habilitação
- `IsMoving`: Flag indicando se está em movimento
- `MovingFront`: Flag indicando direção
- `StepCount`: Contador de passos
- `DesiredSteps`: Número de passos desejados
- `Timer`: Handle do timer ESP32

**Eventos:**
- `OnFinishStepping`: Disparado quando o movimento termina

**Constantes:**
- `DefaultSpeed`: Velocidade padrão (200)

#### `Battery`
Classe para monitorar a tensão da bateria usando ADC.

**Métodos principais:**
- `GetVoltage()`: Retorna a tensão da bateria em mV

**Características:**
- Usa ADC1 do ESP32
- Calibração automática do ADC
- Média de múltiplas amostras (4 amostras)
- Atenuação de 11dB para maior range
- Conversão automática: leitura ADC * 2 = tensão da bateria

**Constantes:**
- `DEFAULT_VREF`: Referência padrão de tensão (1100mV)
- `NO_OF_SAMPLES`: Número de amostras para média (4)
- `ATTENUATION`: Atenuação do ADC (ADC_ATTEN_DB_11)
- `ADC_UNIT`: Unidade ADC (ADC_UNIT_1)
- `ADC_WIDTH`: Largura do ADC (ADC_BITWIDTH_12)

### Dependências
- `Utility`: Utilitários gerais (Event, Utility)
- `ErrorCodes`: Sistema de códigos de erro

### Exemplo de Uso

```cpp
// Motor de passo
Stepper motor(GPIO_NUM_18, GPIO_NUM_19, GPIO_NUM_5);
motor.Init();
motor.SetDirection(true);
motor.Move(200, 100); // 200 passos a 100 Hz

// Bateria
Battery battery(ADC_CHANNEL_0);
uint32_t voltage = battery.GetVoltage(); // mV
ESP_LOGI("APP", "Tensão da bateria: %lu mV", voltage);
```

---

## ErrorCodes

### Descrição
Sistema completo de gerenciamento de códigos de erro padronizado. Permite definir, recuperar e gerenciar erros de forma consistente em toda a aplicação.

### Classes Principais

#### `ErrorCode`
Classe que representa um código de erro com nome, descrição e tipo.

**Métodos principais:**
- `define()`: Define um novo código de erro (estático)
- `get()`: Obtém um código de erro pelo nome (estático)
- `initialize()`: Inicializa o banco de dados de erros (estático)
- `name()`: Retorna o nome do erro
- `description()`: Retorna a descrição do erro
- `type()`: Retorna o tipo do erro
- `isValid()`: Verifica se o erro é válido
- `log()`: Registra o erro no console

**Operadores:**
- `==`: Comparação de igualdade
- `!=`: Comparação de desigualdade
- `<<`: Operador de saída para streams

**Enum `ErrorCodeType`:**
- `General`: Erros gerais
- `Network`: Erros de rede
- `WiFi`: Erros específicos de WiFi
- `Socket`: Erros de socket
- `File`: Erros de arquivo
- `Memory`: Erros de memória
- `Hardware`: Erros de hardware
- `User`: Erros relacionados a usuários
- `Storage`: Erros de armazenamento
- `Project`: Erros específicos do projeto
- `Communication`: Erros de comunicação

#### Classes de Códigos de Erro Específicos

Cada módulo define seus próprios códigos de erro:

- **`BluetoothErrorCodes`**: Erros relacionados a Bluetooth
- **`CommunicationErrorCodes`**: Erros de comunicação
- **`FileErrorCodes`**: Erros de arquivo
- **`GeneralErrorCodes`**: Erros gerais (inclui `None`, `ArgumentError`, etc.)
- **`HardwareErrorCodes`**: Erros de hardware
- **`NetworkErrorCodes`**: Erros de rede
- **`StorageErrorCodes`**: Erros de armazenamento
- **`UserErrorCodes`**: Erros relacionados a usuários
- **`WifiErrorCodes`**: Erros específicos de WiFi

**`CommonErrorCodes`**: Códigos de erro comuns usados em múltiplos módulos:
- `None`: Sem erro
- `ArgumentError`: Erro de argumento
- `FileNotFound`: Arquivo não encontrado
- `FileExists`: Arquivo já existe
- `FileOpenError`: Erro ao abrir arquivo
- `FileIsEmpty`: Arquivo vazio
- `KeyNotFound`: Chave não encontrada
- `KeyAlreadyExists`: Chave já existe
- `ListIsEmpty`: Lista vazia
- `NoFreeConnections`: Sem conexões livres
- `StorageReadError`: Erro de leitura de armazenamento
- `StorageWriteError`: Erro de escrita de armazenamento

### Dependências
- `Utility`: Utilitários gerais

### Exemplo de Uso

```cpp
// Inicializar sistema de erros
ErrorCode::initialize();

// Definir um novo erro
ErrorCode myError = ErrorCode::define(
    "MyError",
    "Descrição do meu erro",
    ErrorCodeType::General
);

// Obter um erro existente
ErrorCode err = ErrorCode::get("FileNotFound");

// Verificar e usar
if (err != CommonErrorCodes::None) {
    err.log("TAG", ESP_LOG_ERROR);
    return err;
}
```

---

## IoUtility

### Descrição
Módulo de utilitários para operações de entrada/saída, incluindo controle de LED e filtragem de entradas digitais.

### Classes Principais

#### `LedBlinker`
Classe para controlar um LED com piscada automática.

**Métodos principais:**
- `Update()`: Atualiza o estado do LED (deve ser chamado periodicamente)

**Propriedades públicas:**
- `Level`: Nível atual do LED (0 ou 1)
- `LastBlink`: Tick da última piscada
- `Interval`: Intervalo entre piscadas (em ticks)
- `Gpio`: Número do pino GPIO

**Características:**
- Alterna o estado do LED automaticamente
- Baseado em ticks do FreeRTOS
- Configuração de intervalo em milissegundos

#### `FilteredInput`
Classe para filtrar entradas digitais com debounce.

**Métodos principais:**
- `Update()`: Atualiza o estado da entrada filtrada
- `IsFiltered()`: Retorna o estado filtrado atual
- `GetValue()`: Retorna o valor atual da entrada
- `Changed()`: Verifica se o valor mudou desde a última chamada

**Características:**
- Debounce configurável em milissegundos
- Função de leitura customizável via callback
- Thread-safe

#### `FilteredInputEx`
Classe estendida de `FilteredInput` com eventos de pressionar/soltar/clicar.

**Métodos principais:**
- `ExUpdate()`: Atualiza o estado com detecção de eventos

**Eventos:**
- `PressedEvent`: Disparado quando o botão é pressionado
- `ReleasedEvent`: Disparado quando o botão é solto
- `ClickedEvent`: Disparado quando há um clique rápido

**Propriedades:**
- `ActiveLow`: Define se a entrada é ativa em nível baixo (padrão: true)

**Métodos estáticos:**
- `SetTaskStackSize()`: Define o tamanho da pilha da tarefa de processamento
- `GetInstances()`: Retorna lista de todas as instâncias

**Características:**
- Executa em tarefa separada do FreeRTOS
- Detecção automática de eventos de botão
- Suporte a múltiplas instâncias

### Dependências
- `Utility`: Utilitários gerais (Event, Utility)
- `ErrorCodes`: Sistema de códigos de erro

### Exemplo de Uso

```cpp
// LED piscante
LedBlinker led(500, GPIO_NUM_2); // Pisca a cada 500ms no GPIO 2
led.Update(); // Chamar periodicamente no loop

// Entrada filtrada
FilteredInputEx button(
    []() { return gpio_get_level(GPIO_NUM_0); },
    50 // Debounce de 50ms
);

button.PressedEvent.addHandler([](FilteredInput* input, void* data) {
    ESP_LOGI("APP", "Botão pressionado!");
});

button.ExUpdate(); // Processado automaticamente em tarefa separada
```

---

## JsonModels

### Descrição
Módulo para serialização e deserialização de dados JSON usando a biblioteca nlohmann/json. Fornece classes base e modelos específicos para diferentes tipos de dados.

### Classes Principais

#### `BaseJsonData`
Classe base abstrata para todos os modelos JSON.

**Métodos virtuais puros:**
- `toJson()`: Converte o objeto para string JSON
- `fromJson()`: Popula o objeto a partir de um objeto JSON

**Métodos implementados:**
- `fromString()`: Popula o objeto a partir de uma string JSON

#### `BaseJsonDataError`
Classe base para modelos JSON que incluem informações de erro.

**Propriedades:**
- `ErrorMessage`: Código de erro (ErrorCode)

**Métodos:**
- `toJson()`: Converte incluindo informações de erro
- `getPartialJson()`: Obtém JSON parcial com informações de erro

#### `BaseListJsonDataBasic`
Classe base para modelos JSON que representam listas.

**Propriedades:**
- `End`: Flag indicando fim da lista
- `Begin`: Flag indicando início da lista
- `Index`: Índice atual na lista

#### `BaseListJsonData<Tkey, Tvalue>`
Classe template base para listas com tipos específicos de chave e valor.

**Métodos virtuais puros:**
- `fromPair()`: Popula o objeto a partir de um par chave-valor

#### `UuidInfoJsonData`
Modelo JSON para informações de UUID.

**Propriedades:**
- `NotifyUUID`: UUID da característica de notificação
- `ServiceUUID`: UUID do serviço
- `WriteUUID`: UUID da característica de escrita

#### Modelos Condicionais (se `USER_MANAGEMENT_ENABLED`)

**`User`**: Modelo para dados de usuário
- `Name`: Nome do usuário
- `Password`: Senha
- `Email`: Email
- `IsConfirmed`: Flag de confirmação
- `IsAdmin`: Flag de administrador
- `isValid()`: Valida os dados do usuário
- `toPureJson()`: Converte para JSON puro (sem erro)

**`UserListJsonData`**: Modelo para lista de usuários
- `UserName`: Nome do usuário
- `UserJson`: Objeto User

**`LoginTryResultJson`**: Modelo para resultado de tentativa de login
- `IsAdmin`: Flag indicando se é administrador

**`SignUpResultJson`**: Modelo para resultado de cadastro (herda de `LoginTryResultJson`)

### Operadores de Stream
- `<<`: Operador de saída para streams
- `>>`: Operador de entrada para streams

### Dependências
- `Utility`: Utilitários gerais
- `ErrorCodes`: Sistema de códigos de erro
- `johboh/nlohmann-json`: Biblioteca JSON externa

### Exemplo de Uso

```cpp
// Criar e serializar
UuidInfoJsonData uuidInfo;
uuidInfo.ServiceUUID = "12345678-1234-1234-1234-123456789abc";
uuidInfo.WriteUUID = "87654321-4321-4321-4321-cba987654321";
uuidInfo.NotifyUUID = "abcdef12-3456-7890-abcd-ef1234567890";

std::string json = uuidInfo.toJson();
// {"ServiceUUID":"12345678-...", "WriteUUID":"...", "NotifyUUID":"..."}

// Deserializar
UuidInfoJsonData parsed;
parsed.fromString(json);
```

---

## NimBLE

### Descrição
Wrapper C++ para a biblioteca NimBLE do ESP-IDF. Fornece uma interface orientada a objetos para funcionalidades Bluetooth Low Energy.

### Características
- Wrapper C++ sobre a API C do NimBLE
- Integração com o sistema de componentes ESP-IDF
- Suporte a serviços, características e descritores BLE

### Dependências
- `Utility`: Utilitários gerais

### Nota
Este módulo é principalmente um wrapper e é usado internamente pelo módulo `BluetoothServer`. Para uso direto, consulte a documentação do NimBLE do ESP-IDF.

---

## Storage

### Descrição
Módulo completo para gerenciamento de armazenamento persistente, incluindo NVS (Non-Volatile Storage), Flash externa e cartão SD.

### Classes Principais

#### `Storage`
Classe principal para operações de armazenamento em sistema de arquivos.

**Métodos principais:**
- `initialize()`: Inicializa o sistema de armazenamento (tenta filesystem, fallback para NVS)
- `isFileSystemAvailable()`: Verifica se o sistema de arquivos está disponível
- `eraseData()`: Apaga todos os dados
- `deleteFile()`: Deleta um arquivo
- `copyFile()`: Copia um arquivo
- `getStatus()`: Obtém status do armazenamento (espaço livre/total)
- `storeKeyValue()`: Armazena um par chave-valor (template)
- `readKeyValue()`: Lê um valor por chave (template)
- `readOrCreateKeyValue()`: Lê ou cria um par chave-valor (template)
- `getEntriesFromFile()`: Obtém todas as entradas de um arquivo (template)
- `storeConfig()`: Armazena configuração
- `loadConfig()`: Carrega configuração

**Métodos condicionais (se `USER_MANAGEMENT_ENABLED`):**
- `storeUser()`: Armazena usuário
- `loadUser()`: Carrega usuário
- `getAllUsers()`: Obtém todos os usuários
- `getEntriesFromUser()`: Obtém entradas de um usuário

**Constantes (`StorageConstants`):**
- `BasePath`: Caminho base para armazenamento ("/storage")
- `UsersFilename`: Nome do arquivo de usuários ("users")
- `ConfigFilename`: Nome do arquivo de configuração ("config")
- `InfoFilename`: Nome do arquivo de informações ("info")

**Estrutura `StorageStatus`:**
- `freeSpace`: Espaço livre em bytes
- `totalSpace`: Espaço total em bytes

**Características:**
- Suporte a sistema de arquivos (SD Card ou Flash) com fallback para NVS
- Formato de arquivo: "chave=valor" (um por linha)
- Validação de nomes de arquivo reservados
- Thread-safe

#### `NVS`
Classe para interagir com o Non-Volatile Storage do ESP32.

**Métodos principais:**
- `initialize()`: Inicializa a memória flash NVS
- `eraseData()`: Apaga todos os dados da partição NVS
- `storeValue()`: Armazena um valor (template, suporta vários tipos)
- `readValue()`: Lê um valor (template)
- `getEntriesFromNamespace()`: Obtém todas as entradas de um namespace (template)

**Tipos suportados:**
- `std::string`
- Tipos inteiros: `int8_t`, `uint8_t`, `int16_t`, `uint16_t`, `int32_t`, `uint32_t`, `int64_t`, `uint64_t`
- `int`, `unsigned int`

**Características:**
- Organização por namespaces
- Suporte a overwrite ou proteção contra sobrescrita
- Iteração sobre todas as entradas de um namespace

#### `Flash`
Classe para gerenciar memória flash externa.

**Métodos principais:**
- `initialize()`: Inicializa e monta a flash usando FATFS
- `getWearLevellingHandle()`: Obtém o handle do wear levelling

**Características:**
- Montagem automática do sistema de arquivos FATFS
- Formatação automática se necessário
- Wear levelling para prolongar vida útil

#### `SdCard`
Classe para gerenciar cartão SD via SPI.

**Métodos principais:**
- `initialize()`: Inicializa e monta o cartão SD

**Parâmetros:**
- `mountPoint`: Ponto de montagem (ex: "/sdcard")
- `formatIfFailed`: Se true, formata o cartão se a montagem falhar

**Características:**
- Suporte a cartões SD via interface SPI
- Formatação opcional em caso de falha
- Integração com sistema de arquivos FATFS

### Dependências
- `Utility`: Utilitários gerais
- `JsonModels`: Modelos JSON (para operações com usuários)
- `ErrorCodes`: Sistema de códigos de erro

### Exemplo de Uso

```cpp
// Inicializar armazenamento
ErrorCode err = Storage::initialize();
if (err != CommonErrorCodes::None) {
    ESP_LOGE("APP", "Falha ao inicializar armazenamento");
    return;
}

// Armazenar valor
err = Storage::storeKeyValue("chave1", 12345, "config");
err = Storage::storeKeyValue("chave2", "valor", "config");

// Ler valor
int valor;
err = Storage::readKeyValue("chave1", valor, "config");

// Ler ou criar com valor padrão
int valorPadrao = 100;
err = Storage::readOrCreateKeyValue("chave3", valorPadrao, "config", "chave3");

// NVS direto
NVS::initialize();
NVS::storeValue("namespace", "chave", 42, true);
int valorNVS;
NVS::readValue("namespace", "chave", valorNVS);
```

---

## UserManaging

### Descrição
Módulo para gerenciamento de usuários, incluindo autenticação, autorização e controle de acesso. Requer que `USER_MANAGEMENT_ENABLED` esteja definido.

### Classes Principais

#### `UserManager`
Classe principal para gerenciamento de usuários.

**Métodos principais:**
- `Login()`: Processa tentativa de login
- `Logoff()`: Faz logout de um usuário
- `SaveUser()`: Salva um usuário no armazenamento
- `LoadUser()`: Carrega um usuário do armazenamento
- `SignUp()`: Processa cadastro de novo usuário
- `GetUsersWaitingForApproval()`: Obtém lista de usuários aguardando aprovação
- `ApproveUser()`: Aprova um usuário pendente
- `ClearUsers()`: Limpa todos os usuários
- `CheckPassword()`: Verifica senha de um usuário
- `SendLoginTryResult()`: Envia resultado de tentativa de login
- `CreateManager()`: Cria instância do gerenciador
- `Disconnect()`: Desconecta um usuário

**Códigos de erro específicos:**
- `NotConfirmed`: Usuário precisa de liberação do administrador
- `UserNotFound`: Usuário não encontrado
- `WrongPassword`: Senha incorreta
- `AdminNotRegistered`: Administrador não cadastrado
- `NoUsersRegistered`: Nenhum usuário cadastrado

**Características:**
- Sistema de aprovação de usuários
- Primeiro usuário cadastrado vira administrador automaticamente
- Integração com BluetoothServer para autenticação via BLE
- Armazenamento persistente via módulo Storage

#### `ConnectedUser`
Classe que representa um usuário conectado.

**Características:**
- Associação com conexão Bluetooth
- Estado de autenticação
- Informações do usuário

#### `SimpleUser`
Classe para representação simplificada de usuário.

#### `SimpleUserManager`
Gerenciador simplificado de usuários.

### Dependências
- `Storage`: Armazenamento persistente
- `JsonModels`: Modelos JSON (User, LoginTryResultJson, etc.)
- `BluetoothServer`: Servidor Bluetooth para autenticação
- `ErrorCodes`: Sistema de códigos de erro
- `Utility`: Utilitários gerais

### Exemplo de Uso

```cpp
#ifdef USER_MANAGEMENT_ENABLED
// Cadastrar usuário
JsonModels::User novoUsuario;
novoUsuario.Name = "usuario1";
novoUsuario.Password = "senha123";
novoUsuario.Email = "usuario@example.com";
novoUsuario.IsAdmin = false;

ErrorCode err = UserManager::SaveUser(novoUsuario);

// Login via Bluetooth
BluetoothConnection* conn = ...;
UserManager manager;
manager.Login({"usuario1", "senha123"}, conn);

// Verificar se usuário está autenticado
ConnectedUser* user = conn->getUser();
if (user != nullptr) {
    ESP_LOGI("APP", "Usuário autenticado: %s", user->getName().c_str());
}
#endif
```

---

## Utility

### Descrição
Módulo base com utilitários gerais usados por todos os outros módulos. Inclui sistema de eventos, containers thread-safe, padrões de design e utilitários de sistema.

### Classes Principais

#### `Event<Args...>`
Classe template para sistema de eventos/callbacks thread-safe.

**Métodos principais:**
- `addHandler()`: Adiciona um handler ao evento
- `removeHandler()`: Remove um handler do evento
- `trigger()`: Dispara o evento chamando todos os handlers

**Características:**
- Thread-safe com mutex
- Suporte a múltiplos handlers
- Template variadic para argumentos flexíveis

#### `Singleton<T>`
Classe template para implementar padrão Singleton.

**Uso:**
```cpp
class MyClass : public Singleton<MyClass> {
    friend class Singleton<MyClass>;
private:
    MyClass(token) {} // Constructor privado com token
public:
    // Métodos públicos
};

// Uso
MyClass* instance = MyClass::getInstance();
```

#### `SafeList<T>`
Container thread-safe para listas.

**Características:**
- Operações thread-safe
- Baseado em std::list
- Mutex interno

#### `SafeMap<K, V>`
Container thread-safe para mapas.

**Características:**
- Operações thread-safe
- Baseado em std::map
- Mutex interno

#### `LockableContainer<T>`
Container genérico com lock/unlock.

**Características:**
- Wrapper para qualquer container
- Métodos lock() e unlock()
- Acesso thread-safe

#### `Timeout`
Classe para gerenciar timeouts.

**Características:**
- Verificação de timeout
- Reset de timeout
- Baseado em ticks do FreeRTOS

#### `Utility`
Classe com funções utilitárias diversas.

**Métodos principais:**
- `SetOutput()`: Configura pino como saída
- `SetInput()`: Configura pino como entrada
- `CreateAndProfile()`: Cria tarefa FreeRTOS com profiling

#### `CrossPlatformUtility`
Utilitários para compatibilidade entre plataformas (ESP32, STM32).

**Características:**
- Abstração de diferenças entre plataformas
- Funções comuns para ambas

#### `Exceptions`
Definições de exceções customizadas.

**Características:**
- Exceções específicas do projeto
- Herança de std::exception

### Dependências
- FreeRTOS (para mutex e tarefas)
- ESP-IDF (para GPIO e sistema)

### Exemplo de Uso

```cpp
// Evento
Event<int, std::string> myEvent;
myEvent.addHandler([](int value, std::string text) {
    ESP_LOGI("APP", "Evento: %d, %s", value, text.c_str());
});
myEvent.trigger(42, "teste");

// Singleton
class MySingleton : public Singleton<MySingleton> {
    friend class Singleton<MySingleton>;
private:
    MySingleton(token) {}
public:
    void doSomething() {}
};

MySingleton::getInstance()->doSomething();

// SafeList
SafeList<int> safeList;
safeList.push_back(1);
safeList.push_back(2);
```

---

## Wifi

### Descrição
Módulo completo para funcionalidades WiFi, incluindo conexão em modo estação, servidor Telnet e atualizações OTA (Over-The-Air).

### Classes Principais

#### `Wifi`
Classe principal que combina todas as funcionalidades WiFi.

**Métodos principais:**
- `begin()`: Inicializa conexão WiFi com SSID e senha
- `handle()`: Processa conexões Telnet e dados recebidos
- `printStatus()`: Imprime status da conexão

**Características:**
- Herda de `WifiConnection`, `WifiTelnet` e `WifiOta`
- Interface unificada para todas as funcionalidades WiFi

#### `WifiConnection`
Classe para gerenciar conexão WiFi em modo estação (STA).

**Métodos principais:**
- `connect()`: Conecta a uma rede WiFi
- `disconnect()`: Desconecta da rede
- `isConnected()`: Verifica se está conectado
- `getSSID()`: Obtém o SSID da rede conectada
- `getIPAddress()`: Obtém o endereço IP
- `sendRawData()`: Envia dados brutos via WiFi
- `scan()`: Escaneia redes WiFi disponíveis
- `getId()`: Obtém o ID da conexão
- `setWifiClient()`: Define o cliente WiFi a usar

**Eventos:**
- `onConnect`: Disparado quando conecta à rede WiFi

**Características:**
- Herda de `BaseConnection`
- Integração com sistema de eventos do ESP-IDF
- Retry automático de conexão
- Suporte a múltiplas tentativas

#### `WifiTelnet`
Classe para servidor Telnet sobre WiFi.

**Métodos principais:**
- Métodos herdados de funcionalidades Telnet

**Características:**
- Servidor Telnet para acesso remoto
- Integração com libtelnet
- Suporte a comandos remotos

#### `WifiOta`
Classe para atualizações Over-The-Air.

**Métodos principais:**
- Métodos para atualização de firmware via WiFi

**Características:**
- Atualização de firmware sem conexão física
- Suporte a HTTP/HTTPS
- Verificação de integridade

#### `WifiClient`
Classe para cliente WiFi (comunicação TCP/UDP).

**Características:**
- Cliente TCP/UDP
- Envio e recebimento de dados
- Integração com WifiConnection

#### `WirelessDevice`
Classe base para dispositivos sem fio.

**Características:**
- Interface comum para dispositivos sem fio
- Abstração de funcionalidades comuns

### Dependências
- `Connection`: Gerenciamento de conexões base
- `Utility`: Utilitários gerais
- `ErrorCodes`: Sistema de códigos de erro
- `esp_wifi`: Componente WiFi do ESP-IDF
- `esp_netif`: Componente de rede do ESP-IDF
- `libtelnet`: Biblioteca Telnet (submódulo)

### Exemplo de Uso

```cpp
// Inicializar WiFi
Wifi wifi;
wifi.begin("MinhaRede", "senha123");

// Verificar conexão
if (wifi.isConnected()) {
    IPAddress ip = wifi.getIPAddress();
    ESP_LOGI("APP", "Conectado! IP: %s", ip.toString().c_str());
}

// Processar Telnet e OTA
wifi.handle();

// Escanear redes
wifi_ap_record_t ap_list[10];
int count = wifi.scan(ap_list, 10);
ESP_LOGI("APP", "Encontradas %d redes", count);
```

---

## Resumo de Dependências

```
Utility (base)
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

## Notas Importantes

1. **Compilação Condicional**: Alguns módulos têm funcionalidades condicionais baseadas em `#ifdef`:
   - `USER_MANAGEMENT_ENABLED`: Habilita gerenciamento de usuários
   - `USE_SDCARD`: Habilita suporte a cartão SD

2. **Thread Safety**: Muitos módulos são thread-safe usando mutex do FreeRTOS.

3. **Error Handling**: Todos os módulos usam o sistema `ErrorCode` para tratamento consistente de erros.

4. **Eventos**: Muitos módulos usam o sistema `Event` para callbacks assíncronos.

5. **Singleton Pattern**: Algumas classes usam o padrão Singleton via classe base `Singleton<T>`.

6. **Template Methods**: Vários métodos são templates para suportar diferentes tipos de dados.

---

## Versão
Esta documentação foi gerada para a versão do projeto compatível com ESP-IDF v6.0 ou superior.
