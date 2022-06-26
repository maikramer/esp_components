//
// Created by maikeu on 23/06/22.
//

#include "Telnet.h"
#include "Wifi.h"
#include "IPAddress.h"

Event<TelnetClientData &, std::string> Telnet::OnDataReceived{};
SimpleEvent<IPAddress> Telnet::OnClientConnected{};
SimpleEvent<IPAddress> Telnet::OnClientDisconnected;
TelnetOptions Telnet::Options{};
TelnetClientData Telnet::_telnetUsers[2]{{},
                                         {}};

static void TelnetTsk(void *arg) {
    auto *client = (TelnetClientData *) arg;
    Telnet::ListenForClients(*client);
    vTaskDelete(nullptr);
}

void Telnet::GotIp() {
    Utility::CreateAndProfile("TelnetTsk", TelnetTsk, 6144, MEDIUM_PRIORITY, 1,
                              &_telnetUsers[0]);
    if (Options.HasPort24Server) {
        Utility::CreateAndProfile("TelnetTsk2", TelnetTsk, 6144, MEDIUM_PRIORITY, 1,
                                  &_telnetUsers[1]);
    }
    ESP_LOGI("Telnet", "Telnet Iniciada");
}


void Telnet::Start() {
    _telnetUsers[0].Port = 23;
    _telnetUsers[1].Port = 24;
    if (MK::Wifi::GetState() == WifiState::GotIp) {
        GotIp();
    }

    MK::Wifi::OnGotIp.AddListener([] {
        GotIp();
    });
}

int Telnet::SendData(TelnetClientData &client, uint8_t *buffer, size_t size) {
    if (client.Handle != nullptr) {
        telnet_send(client.Handle, (char *) buffer, size);
        return size;
    } else {
        return 0;
    }
}

[[maybe_unused]] int Telnet::SendString(TelnetClientData &client, std::string str) {
    str.append("\n");
    return SendData(client, (uint8_t *) str.c_str(), strlen(str.c_str()) + 1);
}

void Telnet::TelnetHandler(telnet_t *thisTelnet, telnet_event_t *event, void *clientData) {
    int rc;
#ifdef LOOKUP_TABLE
    ESP_LOGI(__FUNCTION__, "telnet event: %s", EventLookup(event->type).c_str());
#endif
    auto *client = (TelnetClientData *) clientData;
    auto *buf = new char[event->data.size];
    switch (event->type) {
        case TELNET_EV_SEND:
            rc = send(client->Socket, event->data.buffer, event->data.size, 0);
            if (rc < 0) {
                ESP_LOGE(__FUNCTION__, "send: %d (%s)", errno, strerror(errno));
            }
            break;

        case TELNET_EV_DATA:
            ESP_LOGD(__FUNCTION__, "received data, len=%d", event->data.size);
            /**
             * Here is where we would want to handle newly received data.
             * The data receive is in event->data.buffer of size
             * event->data.size.
             */

            memcpy(buf, event->data.buffer, event->data.size);
            buf[event->data.size - 1] = '\0';
            if (Options.CleanCRLF) {
                auto cr = strchr(buf, '\r');
                if (cr != nullptr) {
                    *cr = '\0';
                }
                auto lf = strchr(buf, '\n');
                if (lf != nullptr) {
                    *lf = '\0';
                }
            }

            OnDataReceived.FireEvent(*client, std::string(buf));
            break;

        default:
            break;
    } // End of switch event type
    delete[](buf);
}

void Telnet::ProcessTelnet(TelnetClientData &client) {
    static const telnet_telopt_t my_telopts[] = {
            {TELNET_TELOPT_ECHO,      TELNET_WILL, TELNET_DONT},
            {TELNET_TELOPT_TTYPE,     TELNET_WILL, TELNET_DONT},
            {TELNET_TELOPT_COMPRESS2, TELNET_WONT, TELNET_DO},
            {TELNET_TELOPT_ZMP,       TELNET_WONT, TELNET_DO},
            {TELNET_TELOPT_MSSP,      TELNET_WONT, TELNET_DO},
            {TELNET_TELOPT_BINARY,    TELNET_WILL, TELNET_DO},
            {TELNET_TELOPT_NAWS,      TELNET_WILL, TELNET_DONT},
            {-1, 0, 0}
    };
    client.Handle = telnet_init(my_telopts, TelnetHandler, 0, &client);
    uint8_t buffer[1024];
    while (1) {
        ssize_t len = recv(client.Socket, (char *) buffer, sizeof(buffer), 0);
        if (len <= 0) {
            break;
        }
        telnet_recv(client.Handle, (char *) buffer, len);
    }
    ESP_LOGI("Telnet", "Cliente %s Desconectado", client.Ip.toString().c_str());
    OnClientDisconnected.FireEvent(client.Ip);
    closesocket(client.Socket);
    telnet_free(client.Handle);
    client.Clear();
}

void Telnet::ListenForClients(TelnetClientData &client) {
    int serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    ESP_LOGI(__FUNCTION__, "PORT:%d", client.Port);
    serverAddr.sin_port = htons((int16_t) client.Port);
    int rc = bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    if (rc < 0) {
        ESP_LOGE(__FUNCTION__, "bind: %d (%s)", errno, strerror(errno));
        return;
    }

    rc = listen(serverSocket, 5);
    if (rc < 0) {
        ESP_LOGE(__FUNCTION__, "listen: %d (%s)", errno, strerror(errno));
        return;
    }

    while (1) {
        socklen_t len = sizeof(serverAddr);
        rc = accept(serverSocket, (struct sockaddr *) &serverAddr, &len);
        if (rc < 0) {
            ESP_LOGE(__FUNCTION__, "accept: %d (%s)", errno, strerror(errno));
            return;
        }
        client.Socket = rc;
        struct sockaddr_storage addr;
        socklen_t namelen = sizeof addr;
        getpeername(rc, (struct sockaddr *) &addr, &namelen);
        struct sockaddr_in *s = (struct sockaddr_in *) &addr;
        auto ip = IPAddress((uint32_t) (s->sin_addr.s_addr));
        client.Ip = ip;
        ESP_LOGI("Telnet", "Cliente %s Conectado", ip.toString().c_str());
        ESP_LOGI(__FUNCTION__, "%s", client.toString().c_str());
        OnClientConnected.FireEvent(ip);
        ProcessTelnet(client);
    }
}

int Telnet::Println(TelnetClientData &client, const char *fmt, ...) {
    va_list va;
    va_start(va, fmt);
    auto ret = telnet_vprintf(client.Handle, fmt, va);
    va_end(va);
    return ret;
}

#ifdef LOOKUP_TABLE
std::string EventLookupTable(telnet_event_type_t type) {
    switch (type) {
        case TELNET_EV_COMPRESS:
            return "TELNET_EV_COMPRESS";
        case TELNET_EV_DATA:
            return "TELNET_EV_DATA";
        case TELNET_EV_DO:
            return "TELNET_EV_DO";
        case TELNET_EV_DONT:
            return "TELNET_EV_DONT";
        case TELNET_EV_ENVIRON:
            return "TELNET_EV_ENVIRON";
        case TELNET_EV_ERROR:
            return "TELNET_EV_ERROR";
        case TELNET_EV_IAC:
            return "TELNET_EV_IAC";
        case TELNET_EV_MSSP:
            return "TELNET_EV_MSSP";
        case TELNET_EV_SEND:
            return "TELNET_EV_SEND";
        case TELNET_EV_SUBNEGOTIATION:
            return "TELNET_EV_SUBNEGOTIATION";
        case TELNET_EV_TTYPE:
            return "TELNET_EV_TTYPE";
        case TELNET_EV_WARNING:
            return "TELNET_EV_WARNING";
        case TELNET_EV_WILL:
            return "TELNET_EV_WILL";
        case TELNET_EV_WONT:
            return "TELNET_EV_WONT";
        case TELNET_EV_ZMP:
            return "TELNET_EV_ZMP";
    }
    return "Unknown type";
}
#endif
