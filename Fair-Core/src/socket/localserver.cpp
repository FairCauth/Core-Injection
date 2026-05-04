#include "localserver.h"
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iostream>
#include <string>
#include <thread>
#pragma comment(lib, "ws2_32.lib")
#include "..\ui\panel.h"

static SOCKET g_serverSock = INVALID_SOCKET;
static SOCKET g_clientSock = INVALID_SOCKET;
static std::string g_recvBuf;  // 全局拼接缓冲区

static void server_thread() {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    g_serverSock = socket(AF_INET, SOCK_STREAM, 0);

    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(9999);
    addr.sin_addr.s_addr = INADDR_ANY;

    bind(g_serverSock, (sockaddr*)&addr, sizeof(addr));
    listen(g_serverSock, 1);

    sockaddr_in clientAddr{};
    int clientAddrLen = sizeof(clientAddr);
    g_clientSock = accept(g_serverSock, (sockaddr*)&clientAddr, &clientAddrLen);

    char ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &clientAddr.sin_addr, ip, sizeof(ip));
    std::cout << "Connected " << ip << "\n";

    char buf[1024];
    while (true) {
        memset(buf, 0, sizeof(buf));
        int received = recv(g_clientSock, buf, sizeof(buf) - 1, 0);
        if (received <= 0) {
            std::cout << "Disconnected\n";
            break;
        }

        g_recvBuf += std::string(buf, received);  // 拼到缓冲区

        // 按 \n 切割，有几条处理几条
        size_t pos;
        while ((pos = g_recvBuf.find('\n')) != std::string::npos) {
            std::string line = g_recvBuf.substr(0, pos);
            g_recvBuf.erase(0, pos + 1);

            if (line.empty()) continue;

            std::string reply = panel::from_client(line);
            if (!reply.empty()) {
                reply += "\n";
                send(g_clientSock, reply.c_str(), (int)reply.size(), 0);
            }
        }
    }

    closesocket(g_clientSock);
    closesocket(g_serverSock);
    g_clientSock = INVALID_SOCKET;
    g_serverSock = INVALID_SOCKET;
    WSACleanup();
}

void localserver::init() {
    std::thread t(server_thread);
    t.detach();
}
void localserver::send(const std::string& message) {
    if (g_clientSock == INVALID_SOCKET) return;
    std::string msg = message + "\n";
    ::send(g_clientSock, msg.c_str(), (int)msg.size(), 0);
}
bool localserver::check_port(int port) {
    WSADATA wsaData;
    WSAStartup(MAKEWORD(2, 2), &wsaData);

    SOCKET sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in addr{};
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr);

    int result = connect(sock, (sockaddr*)&addr, sizeof(addr));
    closesocket(sock);
    WSACleanup();

    return result == 0;  // 连接成功说明端口被占用，Java已启动
}
void localserver::shutdown() {
    if (g_clientSock != INVALID_SOCKET) {
        closesocket(g_clientSock);
        g_clientSock = INVALID_SOCKET;
    }
    if (g_serverSock != INVALID_SOCKET) {
        closesocket(g_serverSock);
        g_serverSock = INVALID_SOCKET;
    }
    WSACleanup();
}