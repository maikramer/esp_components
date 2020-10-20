//
// Created by maikeu on 09/06/2020.
//
#include "projectConfig.h"

#ifdef USER_MANAGEMENT_ENABLED

#include "ConnectedUser.h"

#endif

std::list<uint8_t> ConnectedUser::GetData() {
    return std::list<uint8_t>();
}

NotificationNeeds ConnectedUser::GetNotificationNeeds() {
    return NotificationNeeds::SendNormal;
}

ConnectedUser::ConnectedUser() {
    ESP_LOGI(__FUNCTION__, "User adicionado a lista");
    UserManager::_activeUsers.Push(this);
}

ConnectedUser::~ConnectedUser() {
    ESP_LOGI(__FUNCTION__, "User removido da lista");
    UserManager::_activeUsers.Remove(this);
}
