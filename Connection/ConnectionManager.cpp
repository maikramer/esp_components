//
// Created by maikeu on 08/06/2020.
//

#include <esp_log.h>
#include "ConnectionManager.h"
#include "BaseConnection.h"

// ConnectedUser.h está no componente UserManaging, não é necessário aqui

//#define DEBUG_INFO

SafeList<BaseConnection*> ConnectionManager::_connectionPool;
Event<ConnectionManager*, BaseConnection*> ConnectionManager::onConnect;

ErrorCode ConnectionManager::initialize(size_t numConnections) {
    // ConnectionManager genérico não cria conexões automaticamente
    // As conexões devem ser adicionadas via addConnection()
    // Este método apenas inicializa o pool vazio
    return CommonErrorCodes::None;
}

void ConnectionManager::addConnection(BaseConnection* connection) {
    if (connection != nullptr) {
        _connectionPool.Push(connection);
    }
}

BaseConnection* ConnectionManager::getFreeConnection() {
    if (_connectionPool.IsLocked()) {
        ESP_LOGE(__FUNCTION__, "Falha tentando obter semaforo");
        return nullptr;
    }

    for (auto* conn : _connectionPool) {
        if (conn != nullptr && !conn->isConnected()) {
            return conn;
        }
    }

    ESP_LOGE(__FUNCTION__, "Sem Conexões livres");
    return nullptr;
}

ErrorCode ConnectionManager::connect(uint16_t id) {
    auto* connection = getFreeConnection();
    if (connection != nullptr) {
        // A conexão específica deve implementar o método connect(id)
        // Por enquanto, apenas notificamos que uma conexão foi associada
        onConnect.trigger(nullptr, connection);
        return CommonErrorCodes::None;
    } else {
        ESP_LOGE(__FUNCTION__, "Sem conexões livres");
        return CommonErrorCodes::OperationFailed;
    }
}

void ConnectionManager::disconnect(uint16_t id) {
    auto* conn = getConnectionById(id);
    if (conn == nullptr) return;

#ifdef USER_MANAGEMENT_ENABLED
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Usuario com id %d desconectado", id);
#endif
#endif

    conn->disconnect();
}

BaseConnection* ConnectionManager::getConnectionById(uint16_t id) {
    if (_connectionPool.IsLocked()) {
        ESP_LOGE(__FUNCTION__, "Falha tentando obter semaforo");
        return nullptr;
    }

    for (auto* connection : _connectionPool) {
        if (connection != nullptr && connection->isConnected()) {
            // BaseConnection não tem GetId() diretamente
            // Implementações específicas devem fornecer uma forma de identificar conexões
            // Por enquanto, retornamos a primeira conexão conectada
            // TODO: Adicionar método getId() ao BaseConnection ou usar um mapa de IDs
            return connection;
        }
    }

    ESP_LOGE(__FUNCTION__, "Conexão com Id %d nao encontrado", id);
    return nullptr;
}

void ConnectionManager::sendNotifications() {
    if (_connectionPool.Empty()) {
        return;
    }

    if (_connectionPool.IsLocked()) {
        ESP_LOGE(__FUNCTION__, "Falha tentando obter semaforo");
        return;
    }

    for (auto* connection : _connectionPool) {
        if (connection == nullptr || !connection->isConnected()) {
            continue;
        }

#ifdef USER_MANAGEMENT_ENABLED
        // TODO: Implementar suporte a UserManagement quando necessário
        // Por enquanto, apenas enviamos notificações normais
#endif

        // BaseConnection não tem GetNotificationNeeds() diretamente
        // Por enquanto, enviamos notificações para todas as conexões conectadas
        // TODO: Adicionar suporte a NotificationNeeds ao BaseConnection
    }
}

void ConnectionManager::notifyAll(NotificationNeeds needs) {
    if (_connectionPool.Empty()) {
        return;
    }

    if (_connectionPool.IsLocked()) {
        ESP_LOGE(__FUNCTION__, "Falha tentando obter semaforo");
        return;
    }

    for (auto* connection : _connectionPool) {
        if (connection != nullptr) {
            // BaseConnection não tem SetNotificationNeeds() diretamente
            // TODO: Adicionar suporte a NotificationNeeds ao BaseConnection
        }
    }
}
