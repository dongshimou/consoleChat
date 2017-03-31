#include "server.h"
static messageQueue<msg>m_queue;
static std::map<SOCKET, user>socket_user;
static std::vector<user>users;
static std::atomic<uint32_t>clientNums;
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
            int len = std::strlen(data);
            for (auto &&i : users) {
                if (i.socket != message.origin)
                    send(i.socket,data, len+1, 0);
            }
            delete[] message.data;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(1));
    }
}

void server::revc_data(SOCKET socket) {
    while (true) {
        char buffer[ 8192 ] = { 0 };
        int recvinfo = recv(socket, buffer, sizeof(buffer), 0);
        if (recvinfo < 1) {
            clientNums--;
            show_client();
            return;
        }
        msg m;
        m.origin = socket;
        auto len = std::strlen(buffer);
        if (len == 0)continue;
        m.data = new char[ len+1 ];
        std::strcpy(m.data, buffer);
        m_queue.push(m);
        std::cout << socket_user[ socket ].ip << " : " << buffer << '\n';
    }

}

void server::alive() { }

void server::close(SOCKET socket) { }

void server::show_client() {
    std::cout << "client : " << clientNums << '\n';
}

server::server() noexcept {
    if (!init())
        std::cout << "\nsome error\n";
    else
        std::cout << "\nstart server\n";
}

server::~server() {
    WSACleanup();
    std::cout << "server close";
}

void server::loop(bool multithread) {
    SOCKET acceptfd;
    sockaddr_in clientAddr;
    int nSize;
    clientNums = 0;
    std::thread t_send(send_data);
    t_send.detach();
    std::thread t[ m_clients + 1 ];
    std::cout << "server listen :" << m_hostname << " port :" << m_port << '\n';
    while (clientNums < m_clients) {
        show_client();
        nSize = sizeof(clientAddr);
        acceptfd = accept(m_listen, (sockaddr *)&clientAddr, &nSize);
        if (acceptfd == INVALID_SOCKET) {
            std::cout << "error :" << WSAGetLastError() << '\n';
            return;
        }
        std::cout << "Accepted from client:" << inet_ntoa(clientAddr.sin_addr) << '\n';
        user client("unnamed", inet_ntoa(clientAddr.sin_addr));
        client.socket = acceptfd;
        socket_user[ acceptfd ] = client;
        users.emplace_back(client);
        if (multithread) {
            t[ clientNums ] = std::thread(revc_data, acceptfd);
        } else {
            revc_data(acceptfd);
        }
        clientNums++;
    }
}

