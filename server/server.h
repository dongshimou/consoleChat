#pragma once
#pragma comment(lib,"wsock32.lib")

#include "messageQueue.h"

#include <WinSock2.h>
#include <cstdio>
#include <string>
#include <vector>
#include <iostream>
#include <cstring>
#include <WS2tcpip.h>
#include <fstream>
#include <map>
#include <thread>
#include <future>
#include <chrono>
struct user {
    SOCKET socket;
    std::string name;
    std::string ip;
    bool heart = false;
    user() { }
    user(const char* t_name, const char*t_ip)
        :name(t_name), ip(t_ip) { }
    void alive() { heart = true; }
    bool is_alive() { return heart; }
    void reset() { heart = false; }
};
struct msg {
    SOCKET origin, target;
    char* data = nullptr;
};
class server {
private:
    char* m_hostname = nullptr;
    uint32_t m_port = 2333;
    static const uint32_t m_clients = 10;
    SOCKET m_listen;
    int m_err = 0;
    hostent *m_hent = nullptr;
    sockaddr_in m_serverAddr;
private:
    bool init();
    static void send_data();
    static void revc_data(SOCKET socket);
    static void alive();
    static void show_client();
public:
    explicit server()noexcept;
    server(const server&) = delete;
    server& operator=(const server&) = delete;
    server(server&&other)noexcept = delete;
    ~server();
    void loop(bool multithread = true);
};