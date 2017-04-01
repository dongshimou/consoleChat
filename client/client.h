#pragma once

#pragma comment(lib,"wsock32.lib")
#include <WinSock2.h>
#include <WS2tcpip.h>

#include <thread>
#include <cstdio>
#include <string>
#include <iostream>
#include <cstring>

class client {
private:
    SOCKET m_client;
    sockaddr_in serverAddr;
    hostent *m_host=nullptr;
    char m_hostname[ 64 ] = { 0 };
    uint32_t m_port;

    bool init();
    void send_data();
    static void recv_data(SOCKET socket);
public:
    explicit client()noexcept;
    client&operator=(const client&) = delete;
    client(const client&) = delete;
    client(client&&) = delete;
    ~client();
};