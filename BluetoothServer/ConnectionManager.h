//
// Created by maikeu on 08/06/2020.
//

#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H


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

    static void NotifyAll(bool isImportant);

    static void Init(int noOfConnections);

    static Event<ConnectionManager *, BluetoothConnection *> OnConnect;

private:
    static SafeList<BluetoothConnection *> _connectionPool;//NOLINT

};


#endif //CONNECTIONMANAGER_H
