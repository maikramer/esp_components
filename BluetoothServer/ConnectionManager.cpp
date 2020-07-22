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

#ifdef USER_MANAGEMENT_ENABLED

void ConnectionManager::Init(ConnectedUser *userType, int noOfConnections) {
#else

    void ConnectionManager::Init(int noOfConnections) {
#endif
    for (auto i = 0; i < noOfConnections; i++) {
#ifdef USER_MANAGEMENT_ENABLED
        auto *connection = new BluetoothConnection(userType->CreateNewInstance());
#else
        auto *connection = new BluetoothConnection();
#endif
        connection->Init();
        _connectionPool.Push(connection);
    }
}

void ConnectionManager::Connect(uint16_t conn_id) {
    auto *connection = GetFreeConnection();
    if (connection != nullptr) {
        connection->Setup(conn_id);
    } else {
        BluetoothServer::instance().BleServer->disconnect(conn_id);
    }
}

void ConnectionManager::Disconnect(uint16_t id) {
    auto *conn = GetConnectionById(id);
    if (conn == nullptr) return;
#ifdef USER_MANAGEMENT_ENABLED
    auto *user = conn->GetUser();
    if (user != nullptr) {
        user->Clear();
    }
#ifdef DEBUG_INFO
    ESP_LOGI(__FUNCTION__, "Usuario com id %d desconectado", id);
#endif

#endif
    conn->Free();
}

auto ConnectionManager::GetConnectionById(uint16_t id) -> BluetoothConnection * {
    for (auto *connection : _connectionPool.ReadList()) {
        if (connection->GetId() == id) {
#ifdef DEBUG_INFO
            ESP_LOGI(__FUNCTION__, "Connection Id: %d", id);
#endif
            _connectionPool.EndReadList();
            return connection;
        }
    }

    ESP_LOGE(__FUNCTION__, "Conecção com Id %d nao encontrado", id);
    _connectionPool.EndReadList();
    return nullptr;
}

auto ConnectionManager::GetFreeConnection() -> BluetoothConnection * {
    const static auto *semaphore = xSemaphoreCreateMutex();
    if (xSemaphoreTake(semaphore, 1000) == pdFAIL) {
        ESP_LOGE(__FUNCTION__, "Estouro ao adquirir semaphoro");
        return nullptr;
    }
    BluetoothConnection *ret = nullptr;
    for (auto *conn : _connectionPool.ReadList()) {
        if (conn->IsFree()) {
            ret = conn;
            break;
        }
    }


    if (ret == nullptr) {
        ESP_LOGE(__FUNCTION__, "Sem Conecções livres");
    } else {
        _connectionPool.EndReadList();
    }

    xSemaphoreGive(semaphore);
    return ret;
}

void ConnectionManager::SendNotifications() {
//    ESP_LOGI(__FUNCTION__, "");
    if (!_connectionPool.Empty()) {
        for (auto *connection : _connectionPool.ReadList()) {
            //                ESP_LOGI(__FUNCTION__, "Tentando Enviar para %s", user.User.c_str());
            if (connection == nullptr || connection->IsFree())
                continue;

#ifdef USER_MANAGEMENT_ENABLED
            auto *user = connection->GetUser();
            if (user == nullptr) continue;

            auto isLogged = user->IsLogged;
            if (!isLogged) continue;

            auto state = user->GetNotificationNeeds();
            if (state == NotificationNeeds::NoSend) continue;

#ifdef DEBUG_INFO
#ifdef USER_MANAGEMENT_ENABLED

            ESP_LOGI(__FUNCTION__, "Enviando para %s", user->User.c_str());

#else
            ESP_LOGI(__FUNCTION__, "Enviando para a coneccao %u", connection->GetId());
#endif
#endif
            connection->SendNotifyData(state != NotificationNeeds::SendImportant);

#endif

        }
        _connectionPool.EndReadList();
    }
}

#ifndef USER_MANAGEMENT_ENABLED

void ConnectionManager::NotifyAll(bool isImportant) {
    if (!_connectionPool.Empty()) {
        for (auto *connection : _connectionPool.ReadList()) {
            connection->SetNotificationNeeds(
                    isImportant ? NotificationNeeds::SendImportant : NotificationNeeds::SendNormal);
        }
        _connectionPool.EndReadList();
    }
}

#endif
