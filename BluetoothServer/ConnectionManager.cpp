//
// Created by maikeu on 08/06/2020.
//

#include <esp_log.h>

#include "ConnectionManager.h"
#include "BluetoothServer.h"

#ifdef USER_MANAGEMENT_ENABLED

#include <ConnectedUser.h>

#endif

//#define DEBUG_INFO

SafeList<BluetoothConnection *> ConnectionManager::_connectionPool;//NOLINT

void ConnectionManager::Init(int noOfConnections) {
    for (auto i = 0; i < noOfConnections; i++) {
        auto *connection = new BluetoothConnection();
        connection->Init();
        _connectionPool.Push(connection);
    }
}

void ConnectionManager::Connect(uint16_t conn_id) {
    auto *connection = GetFreeConnection();
    if (connection != nullptr) {
        connection->Connect(conn_id);
    } else {
        ESP_LOGE(__FUNCTION__ , "Sem conexões livres");
        BluetoothServer::instance().BleServer->disconnect(conn_id);
    }
}

void ConnectionManager::Disconnect(uint16_t id) {
    auto *conn = GetConnectionById(id);
    if (conn == nullptr) return;
#ifdef USER_MANAGEMENT_ENABLED

#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Usuario com id %d desconectado", id);
#endif

#endif
    conn->Disconnect();
}

auto ConnectionManager::GetConnectionById(uint16_t id) -> BluetoothConnection * {
    if (_connectionPool.IsLocked()) {
        ESP_LOGE(__FUNCTION__, "Falha tentando obter semaforo");
        return nullptr;
    }
    for (auto connection : _connectionPool) {
        if (connection->GetId() == id) {
#ifdef DEBUG_INFO
            ESP_LOGI(__FUNCTION__, "Connection Id: %d", id);
#endif
            _connectionPool.EndIteration();
            return connection;
        }
    }

    ESP_LOGE(__FUNCTION__, "Conexão com Id %d nao encontrado", id);
    _connectionPool.EndIteration();
    return nullptr;
}

auto ConnectionManager::GetFreeConnection() -> BluetoothConnection * {
    static auto *semaphore = xSemaphoreCreateMutex();//NOLINT
    if (xSemaphoreTake(semaphore, 1000) == pdFAIL) {
        ESP_LOGE(__FUNCTION__, "Estouro ao adquirir semaforo");
        return nullptr;
    }
    BluetoothConnection *ret = nullptr;

    if (_connectionPool.IsLocked()) {
        ESP_LOGE(__FUNCTION__, "Falha tentando obter semaforo");
        return nullptr;
    }

    for (auto *conn : _connectionPool) {
        if (conn->IsFree()) {
            ret = conn;
            break;
        }
    }
    _connectionPool.EndIteration();


    if (ret == nullptr) {
        ESP_LOGE(__FUNCTION__, "Sem Conexões livres");
    }

    xSemaphoreGive(semaphore);
    return ret;
}

void ConnectionManager::SendNotifications() {
    if (!_connectionPool.Empty()) {
        if (_connectionPool.IsLocked()) {
            ESP_LOGE(__FUNCTION__, "Falha tentando obter semaforo");
            return;
        }
        for (auto *connection : _connectionPool) {
            //                ESP_LOGI(__FUNCTION__, "Tentando Enviar para %s", user.User.c_str());
            if (connection == nullptr || connection->IsFree())
                continue;

#ifdef USER_MANAGEMENT_ENABLED
            auto *user = connection->GetUser(true, true);
            if (user == nullptr) continue;

            auto isLogged = user->IsLogged;
            if (!isLogged) continue;

            auto state = user->GetNotificationNeeds();
#else

#ifdef DEBUG_INFO
#ifdef USER_MANAGEMENT_ENABLED

            ESP_LOGI(__FUNCTION__, "Enviando para %s", user->User.c_str());

#else
            ESP_LOGI(__FUNCTION__, "Enviando para a coneccao %u", connection->GetId());
#endif
#endif  //DEBUG_INFO
            auto state = connection->GetNotificationNeeds();
#endif //USER_MANAGEMENT_ENABLED
            if (state == NotificationNeeds::NoSend) continue;
            connection->SendNotifyData(state != NotificationNeeds::SendImportant);


        }
        _connectionPool.EndIteration();
    }
}

#ifndef USER_MANAGEMENT_ENABLED

void ConnectionManager::NotifyAll(bool isImportant) {
    if (!_connectionPool.Empty()) {
        if (_connectionPool.IsLocked()) {
            ESP_LOGE(__FUNCTION__, "Falha tentando obter semaforo");
            return;
        }
        for (auto *connection : _connectionPool) {
            connection->SetNotificationNeeds(
                    isImportant ? NotificationNeeds::SendImportant : NotificationNeeds::SendNormal);
        }
        _connectionPool.EndIteration();
    }
}

#endif
