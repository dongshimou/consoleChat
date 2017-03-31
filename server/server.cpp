#include "server.h"
static messageQueue<msg>m_queue;
static std::map<SOCKET, user>socket_user;
static std::map<user, SOCKET>user_socket;
bool server::init() {
    m_hostname = "127.0.0.1";
    m_port = 20000;
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

void server::send_data() {
    while (true) {
        while (!m_queue.empty()) {
            auto message = m_queue.front();
            m_queue.pop();
            auto data = message.data;
            int len = sizeof(data) / sizeof(byte);
            for (auto &&i : user_socket) {
                if (i.second != message.origin)
                    send(i.second, reinterpret_cast<const char*>(data), len, 0);
            }
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

void server::revc_data(SOCKET socket) {
    while (true) {
        char buffer[ 8192 ] = { 0 };
        int recvinfo = recv(socket, buffer, sizeof(buffer), 0);
        if (recvinfo < 1)return;
        msg m;
        auto len = std::strlen(buffer);
        m.data = new byte[ len ];
        std::strcpy(reinterpret_cast<char*>(m.data), buffer);
        m_queue.push(std::move(m));
    }

}

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
    std::thread t_send(send_data);
    t_send.detach();
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
            t[ clientNums ] = std::thread(revc_data, acceptfd);
        } else {
            revc_data(acceptfd);
        }
        clientNums++;
    }
}

