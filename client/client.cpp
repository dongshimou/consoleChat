#include "client.h"

bool client::init() {
    WSADATA wsaData;
    m_err = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (m_err != 0) {
        std::cout << "error is " << m_err << '\n';
        return false;
    }
    hostname = new char[ 64 ];
    std::cout << "input server ip :";
    std::cin >> hostname;
    std::cout << "input server port :";
    std::cin >> port;
    if (atoi(hostname)) {
        int ip_addr = inet_addr(hostname);
        m_host = gethostbyaddr((char*)&ip_addr, sizeof(int), AF_INET);
    } else { 
        m_host = gethostbyname(hostname);
    }
    std::cout << hostname << " : " << port << '\n';
    if (!m_host) {
        std::cout << "error: can't resolve host name\n";
        return false;
    }
    serverAddr.sin_family = PF_INET;
    memcpy((char*)&serverAddr.sin_addr, m_host->h_addr, m_host->h_length);
    serverAddr.sin_port = htons(port);
    m_client = socket(PF_INET, SOCK_STREAM, 0);

    if (m_client == INVALID_SOCKET) {
        std::cout << "error: no more socket\n";
        return false;
    }
    m_err = connect(m_client, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr));
    if (m_err == INVALID_SOCKET) {
        std::cout << "error: can't connect the server\n";
        return false;
    }
    return true;
}

void client::send_data() { 
    std::thread t_recv(recv_data,m_client);
    t_recv.detach();
    while (true) {
        char buffer[ 4096 ];
        std::cin.getline(buffer, 4096);
        send(m_client, buffer, std::strlen(buffer) + 1, 0);
    }
}

void client::recv_data(SOCKET socket) { 
    while (true) {
        char buffer[ 4096 ] = { 0 };
        int recvinfo = recv(socket, buffer, sizeof(buffer), 0);
        if (recvinfo < 1) {
            std::cout << "recv error\n";
            return;
        }
        std::cout << buffer << '\n';
    }
}

client::client() noexcept { 
    if (init()) {
        send_data();
    } else {
        std::cout << "init error\n";
    }
}

client::~client() { 
    delete hostname;
    closesocket(m_client);
}
