#include "client.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>

Client::Client() {}

bool Client::pingServer(const char* serverIP, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error\n";
        return false;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported\n";
        return false;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection failure\n";
        return false;
    }

    const char* message = "ping";
    send(sock, message, strlen(message), 0);
    std::cout << "Ping message sent\n";

    char buffer[1024] = {0};
    int valread = read(sock, buffer, 1024);
    std::cout << "Server replied: " << buffer << std::endl;

    close(sock);
    return true;
}