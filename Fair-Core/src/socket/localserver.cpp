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

        std::string reply = panel::from_client(std::string(buf));
        if (reply != "") {
            reply += "\n";
            send(g_clientSock, reply.c_str(), reply.size(), 0);
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