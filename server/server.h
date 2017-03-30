#pragma once
#pragma comment(lib,"wsock32.lib")

#include "messageQueue.h"

#include <WinSock2.h>
#include <cstdio>
#include <string>
#include <iostream>
#include <cstring>
#include <WS2tcpip.h>
#include <fstream>
#include <map>
#include <thread>
#include <future>

struct user {
    SOCKET socket;
    std::string name;
    std::string ip;
    user(){ }
    user(const char* t_name,const char*t_ip)
        :name(t_name),ip(t_ip){ }
};
struct msg {
    SOCKET origin, target;
    byte* data;
};
class server {
private:
    std::map<SOCKET,user>socket_user;
    std::map<user, SOCKET>user_socket;

    char* m_hostname=nullptr;
    uint32_t m_port=2333;
    static const uint32_t m_clients=10;
    SOCKET m_listen;
    int m_err=0;
    hostent *m_hent=nullptr;
    sockaddr_in m_serverAddr;

    messageQueue<msg>m_queue;
private:
    bool init();
    static void send();
    static void revc(SOCKET socket);
    static void alive();
    static void close(SOCKET socket);
public:
    explicit server()noexcept;
    server(const server&other) = delete;
    server operator+(const server&other) = delete;
    server(server&&other)noexcept;
    ~server();
    void loop(bool multithread=true);
};