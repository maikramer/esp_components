//
// Created by maikeu on 08/06/2020.
//

#ifndef TOMADA_SMART_CONDO_CONNECTIONMANAGER_H
#define TOMADA_SMART_CONDO_CONNECTIONMANAGER_H


#include <list>
#include "BluetoothConnection.h"
#include "SafeList.h"

class ConnectionManager {

public:
    static auto GetFreeConnection() -> BluetoothConnection *;

    static void Connect(uint16_t conn_id);

    static void SendNotifications();

    static auto GetConnectionById(uint16_t id) -> BluetoothConnection *;

    static void Disconnect(uint16_t id);

#ifndef USER_MANAGEMENT_ENABLED
    static void NotifyAll(bool isImportant);
#endif

#ifdef USER_MANAGEMENT_ENABLED

    static void Init(ConnectedUser *userType, int noOfConnections);

#else
    static void Init(int noOfConnections);
#endif

private:
    static SafeList<BluetoothConnection *> _connectionPool;//NOLINT

};


#endif //TOMADA_SMART_CONDO_CONNECTIONMANAGER_H
