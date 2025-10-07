#include <iostream>
#include <atomic>
#include <thread>
#include <string>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

constexpr int PORT = 8080;
std::atomic<uint64_t> view_count{ 0 };

void handle_client(SOCKET client_socket) {
    char buffer[1024] = { 0 };
    int bytes = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes <= 0) {
        closesocket(client_socket);
        return;
    }

    std::string request(buffer);

    std::string response_body;
    if (request.find("POST /view") == 0) {
        uint64_t current_count = ++view_count;
        response_body = "View added. Current: " + std::to_string(current_count);
    }
    else if (request.find("GET /views") == 0) {
        response_body = "Current views: " + std::to_string(view_count.load());
    }
    else {
        response_body = "Unknown endpoint";
    }

    std::string response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/plain\r\n"
        "Connection: close\r\n\r\n" +
        response_body;

    send(client_socket, response.c_str(), static_cast<int>(response.size()), 0);
    closesocket(client_socket);
}

int main() {
    // 1. Init WinSock
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed\n";
        return 1;
    }

    // 2. Create socket
    SOCKET server_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server_fd == INVALID_SOCKET) {
        std::cerr << "Socket creation failed\n";
        WSACleanup();
        return 1;
    }

    // 3. Bind
    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt));

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) == SOCKET_ERROR) {
        std::cerr << "Bind failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    if (listen(server_fd, SOMAXCONN) == SOCKET_ERROR) {
        std::cerr << "Listen failed\n";
        closesocket(server_fd);
        WSACleanup();
        return 1;
    }

    std::cout << "View counter service running on port " << PORT << std::endl;

    // 4. Accept loop
    while (true) {
        sockaddr_in client_addr{};
        int client_len = sizeof(client_addr);
        SOCKET client_socket = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if (client_socket == INVALID_SOCKET) {
            std::cerr << "Accept failed\n";
            continue;
        }
        std::thread(handle_client, client_socket).detach();
    }

    closesocket(server_fd);
    WSACleanup();
    return 0;
}
