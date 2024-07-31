#include "client.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>
#include <limits>

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

    const char* message = "PING";
    send(sock, message, strlen(message), 0);
    std::cout << "Ping message sent\n";

    char buffer[1024] = {0};
    int valread = read(sock, buffer, 1024);
    
    close(sock);
    
    return (strcmp(buffer, "200") == 0);
}

bool Client::connectToServer(const char* serverIP, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return false;
    }

    sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);

    if (inet_pton(AF_INET, serverIP, &serv_addr.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported" << std::endl;
        return false;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
        std::cerr << "Connection Failed" << std::endl;
        return false;
    }

    std::cout << "Connected to server" << std::endl;

    char buffer[1024] = {0};
    std::string input;

    // Send CONN_REQ
    send(sock, "CONN_REQ", 8, 0);
    int valread = read(sock, buffer, 1024);
    std::cout << "Server: " << buffer << std::endl;

    if (strcmp(buffer, "100") == 0) {
        std::cout << "Enter your username: ";
        std::cin >> input;
        std::string user_set = "USER_SET " + input;
        send(sock, user_set.c_str(), user_set.length(), 0);
        
        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, 1024);
        std::cout << "Server: " << buffer << std::endl;

        if (strcmp(buffer, "USER_OK") != 0) {
            std::cerr << "User authentication failed" << std::endl;
            close(sock);
            return false;
        }
    } else {
        std::cerr << "Unexpected server response" << std::endl;
        close(sock);
        return false;
    }

    // Clear any remaining newline characters from the input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

    while (true) {
        std::cout << "> ";
        std::getline(std::cin, input);
        
        if (input.empty()) {
            std::cout << "Empty input. Please enter a command." << std::endl;
            continue;
        }
        
        if (input == "/exit") {
            send(sock, "DISS", 4, 0);
            break;
        }
        
        std::cout << "Sending: '" << input << "'" << std::endl;
        send(sock, input.c_str(), input.length(), 0);
        
        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, 1024);
        if (valread > 0) {
            std::cout << "Server: '" << buffer << "'" << std::endl;
        } else if (valread == 0) {
            std::cout << "Server closed the connection" << std::endl;
            break;
        } else {
            std::cerr << "Error reading from server" << std::endl;
            break;
        }
    }

    close(sock);
    return true;
}