#include "client.h"

bool client::init() {
    WSADATA wsaData;
    auto err = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (err != 0) {
        std::cout << "error is " << err << '\n';
        return false;
    }
    std::cout << "\ninput server ip :";
#ifdef _DEBUG
    strcpy(m_hostname,"127.0.0.1");
#else
    std::cin >> m_hostname;
#endif // _DEBUG
    std::cout << "\ninput server port :";
#ifdef _DEBUG
    m_port = 20000;
#else
    std::cin >> m_port;
#endif // DEBUG
    if (atoi(m_hostname)) {
        int ip_addr = inet_addr(m_hostname);
        m_host = gethostbyaddr((char*)&ip_addr, sizeof(int), AF_INET);
    } else { 
        m_host = gethostbyname(m_hostname);
    }
    std::cout<<'\n'<< m_hostname << " : " << m_port << '\n';
    if (!m_host) {
        std::cout << "error: can't resolve host name\n";
        return false;
    }
    serverAddr.sin_family = PF_INET;
    memcpy((char*)&serverAddr.sin_addr, m_host->h_addr, m_host->h_length);
    serverAddr.sin_port = htons(m_port);
    m_client = socket(PF_INET, SOCK_STREAM, 0);
    if (m_client == INVALID_SOCKET) {
        std::cout << "error: no more socket\n";
        return false;
    }
    if(connect(m_client, reinterpret_cast<sockaddr*>(&serverAddr), 
                    sizeof(serverAddr))== INVALID_SOCKET){
        std::cout << "error: can't connect the server\n";
        return false;
    }
    return true;
}

void client::send_data() { 
    std::thread t_recv(recv_data,m_client);
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
            std::cout << "recv error,close.\n";
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
    closesocket(m_client);
}
