//
// Created by maikeu on 08/06/2020.
//

#include "ConnectionManager.h"
#include "BluetoothServer.h"
#include "EnergyPlug.h"

#define NO_OF_CONNECTIONS 5
#define DEBUG_INFO

SafeList<BluetoothConnection *> ConnectionManager::_connectionPool;//NOLINT

void ConnectionManager::Init() {
    for (auto i = 0; i < NO_OF_CONNECTIONS; i++) {
        auto *connection = new BluetoothConnection();
        connection->Init();
        _connectionPool.Push(connection);
    }
}

void ConnectionManager::Connect(uint16_t conn_id) {
    auto *connection = GetFreeConnection();
    if (connection != nullptr) {
        connection->Setup(conn_id);
    } else {
        BluetoothServer::GetInstance()->BleServer->disconnect(conn_id);
    }
}

void ConnectionManager::Disconnect(uint16_t id) {
    auto *conn = GetConnectionById(id);
    auto *user = conn->GetUser();
    if (user != nullptr) {
        user->Clear();
#ifdef DEBUG_INFO
        ESP_LOGI(__FUNCTION__, "Usuario com id %d desconectado", id);
#endif
        return;
    }

    ESP_LOGE(__FUNCTION__, "Tentando logoff sem haver um login");
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
    _connectionPool.EndReadList();

    if (ret == nullptr) {
        ESP_LOGE(__FUNCTION__, "Sem Conecções livres");
    }

    xSemaphoreGive(semaphore);
    return ret;
}

void ConnectionManager::SendNotifications() {
    if (!_connectionPool.Empty()) {
        for (auto *connection : _connectionPool.ReadList()) {
            //                ESP_LOGI(__FUNCTION__, "Tentando Enviar para %s", user.User.c_str());
            if (connection == nullptr)
                continue;

            auto *user = connection->GetUser();
            auto isLogged = user->IsLogged;
            auto plugIsValid = user->Plug != nullptr;
            if (!plugIsValid)
                continue;

            auto state = user->Plug->GetState();
            auto stateSending = state == EnergyPlugState::ActiveAndSending || state == EnergyPlugState::Error;
            auto resendNeeded = user->Plug->SendUsageDataIsNeeded();

            if (isLogged && stateSending && resendNeeded) {
                ESP_LOGI(__FUNCTION__, "Enviando para %s", user->User.c_str());
                if (state == EnergyPlugState::Error && !user->Plug->ErrorNotified) {
                    connection->SendUsageData(false);
                    user->Plug->ErrorNotified = true;
                } else {
                    connection->SendUsageData(true);
                }
            }
        }
        vTaskDelay(2);
        _connectionPool.EndReadList();
    }
}
