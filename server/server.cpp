#include "server.h"

bool server::init() {
    m_hostname = "127.0.0.1";
    m_port = 2333;
    WSADATA wsData;
    WORD sockVersion = MAKEWORD(2, 2);
    if (WSAStartup(sockVersion, &wsData) != 0) {
        std::cout << "start up error\n";
        return false;
    }
    m_listen = socket(PF_INET, SOCK_STREAM, 0);
    if (m_listen == INVALID_SOCKET) {
        std::cout << "can't init listen\n";
        return false;
    }
    m_hent = gethostbyname(m_hostname);
    if (!m_hent) {
        std::cout << "can't get host name\n";
        return false;
    }
    m_serverAddr.sin_family = PF_INET;
    m_serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    m_serverAddr.sin_port = htons(m_port);
    if (bind(m_listen, (const sockaddr *)&m_serverAddr,
             sizeof(m_serverAddr)) == INVALID_SOCKET) {
        std::cout << ("can't bind listen port\n");
        return false;
    }
    if (listen(m_listen, SOMAXCONN) == INVALID_SOCKET) {
        printf("can't listen\n");
        return false;
    }
    return true;
}

void server::send() { }

void server::revc(SOCKET socket) { }

void server::alive() { }

void server::close(SOCKET socket) { }

server::server() noexcept {
    if (!init())
        std::cout << "\nsome error\n";
    else
        std::cout << "\nstart server\n";
}

server::server(server && other) noexcept { }

server::~server() {
    WSACleanup();
    std::cout << "server close";
}

void server::loop(bool multithread) {
    SOCKET acceptfd;
    sockaddr_in clientAddr;
    int nSize;
    int clientNums = 0;
    std::thread t_send(send);

    std::thread t[ m_clients + 1 ];
    while (clientNums < m_clients) {
        std::cout << "client : " << clientNums << '\n';
        nSize = sizeof(clientAddr);
        acceptfd = accept(m_listen, (sockaddr *)&clientAddr, &nSize);
        if (acceptfd == INVALID_SOCKET) {
            std::cout << "error :" << WSAGetLastError() << '\n';
            return;
        }
        std::cout << "Accepted from client:" << inet_ntoa(clientAddr.sin_addr) << '\n';
        socket_user[ acceptfd ] = user("unnamed", inet_ntoa(clientAddr.sin_addr));
        if (multithread) {
            t[ clientNums ] = std::thread(revc,acceptfd);
        } else {
            revc(acceptfd);
        }
        clientNums++;
    }
}

