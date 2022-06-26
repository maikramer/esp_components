//
// Created by maikeu on 23/06/22.
//
#include <stdlib.h> // Required for libtelnet.h
#include <esp_log.h>
#include "libtelnet.h"
#include <lwip/def.h>
#include <lwip/sockets.h>
#include <errno.h>
#include <string.h>
#include "sdkconfig.h"
#include "Utility.h"
#include "priorities.h"
#include "Event.h"
#include "IPAddress.h"
#include <SafeList.h>

#ifndef TELNET_H
#define TELNET_H

class TelnetOptions {
public:
    bool CleanCRLF = false;
    bool HasPort24Server = false;
};

class TelnetClientData {
public:
    IPAddress Ip;
    int Socket;
    telnet_t *Handle;
    uint16_t Port;

    void Clear() {
        Ip = IPAddress(0, 0, 0, 0);
        Socket = -1;
        Handle = nullptr;
    }

    std::string toString() {
        using std::string;
        std::stringstream stringstream{};
        stringstream << "IP:" << Ip.toString() << ", S: " << std::to_string(Socket) << ", H: "
                     << (Handle == nullptr ? "null" : "not null");
        return stringstream.str();
    }
};

class Telnet {
public:
    static void Start();

/**
 * Send data to the telnet partner.
 */
    static int SendData(TelnetClientData &client, uint8_t *buffer, size_t size); // SendData


/**
 * Send a vprintf formatted output to the telnet partner.
 */
    [[maybe_unused]] static int SendString(TelnetClientData &client, std::string str); // printf

    static int Println(TelnetClientData &client, const char *fmt, ...);

/**
 * Telnet handler.
 */
    static void TelnetHandler(
            telnet_t *thisTelnet,
            telnet_event_t *event,
            void *clientData); // myTelnetHandler

/**
 * Telnet processing.
 */
    static void ProcessTelnet(TelnetClientData &client);


/**
 * Listen for telnet clients and handle them.
 */
    static void ListenForClients(TelnetClientData &client);

    static Event<TelnetClientData &, std::string> OnDataReceived;
    static SimpleEvent<IPAddress> OnClientConnected;
    static SimpleEvent<IPAddress> OnClientDisconnected;
    static TelnetOptions Options;
private:
    static TelnetClientData _telnetUsers[2];

    static void GotIp();
};


#endif //TELNET_H
