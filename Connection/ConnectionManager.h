#ifndef CONNECTIONMANAGER_H
#define CONNECTIONMANAGER_H

#include <list>
#include "SafeList.h"
#include "BaseConnection.h" // Now includes BaseConnection
#include "Event.h"
#include "CommonErrorCodes.h"

/**
 * @file ConnectionManager.h
 * @brief Defines the ConnectionManager class for managing multiple communication connections.
 */

/**
 * @class ConnectionManager
 * @brief Manages a pool of BaseConnection objects, handling connection, disconnection, and notifications.
 */
class ConnectionManager {
public:
    /**
     * @brief Initializes the ConnectionManager with a specified number of connections.
     * 
     * This method should be called once at the start of the application.
     * 
     * @param numConnections The initial number of connections to create in the pool.
     * @return ErrorCode indicating success or failure of the initialization.
     */
    static ErrorCode initialize(size_t numConnections);

    /**
     * @brief Adds a new BaseConnection to the connection pool.
     *
     * @param connection A pointer to the BaseConnection object to add.
     */
    static void addConnection(BaseConnection* connection);

    /**
     * @brief Gets a free (unused) connection from the pool.
     *
     * @return A pointer to a free BaseConnection, or nullptr if no free connections are available. 
     */
    static BaseConnection* getFreeConnection();

    /**
     * @brief Connects a connection from the pool to a specific ID.
     *
     * This method searches for a free connection and associates it with the provided ID.
     *
     * @param id The ID to connect to.
     * @return ErrorCode indicating success or failure. CommonErrorCodes::NoFreeConnections is returned 
     *         if no free connections are available.
     */
    static ErrorCode connect(uint16_t id);

    /**
     * @brief Disconnects a connection with the specified ID.
     *
     * @param id The ID of the connection to disconnect.
     */
    static void disconnect(uint16_t id);

    /**
     * @brief Gets a connection from the pool by its ID.
     *
     * @param id The ID of the connection to retrieve.
     * @return A pointer to the BaseConnection with the matching ID, or nullptr if not found.
     */
    static BaseConnection* getConnectionById(uint16_t id);

    /**
     * @brief Sends notifications to all connected clients based on their notification needs.
     */
    static void sendNotifications();

    /**
     * @brief Sets the notification needs for all connections to the specified level.
     * 
     * This is used to force all connections to send a notification, regardless of their previous state.
     *
     * @param needs The notification level to set for all connections.
     */
    static void notifyAll(NotificationNeeds needs);

    /**
     * @brief Event triggered when a new connection is established (associated with an ID).
     * 
     * The event handler will receive a pointer to the ConnectionManager object and
     * a pointer to the connected BaseConnection object.
     */
    static Event<ConnectionManager*, BaseConnection*> onConnect;

private:
    static SafeList<BaseConnection*> _connectionPool; /**< Pool of BaseConnection objects. */

    // Private constructor to prevent instantiation (static class)
    ConnectionManager() = delete;
};

#endif //CONNECTIONMANAGER_H