#include "client.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <thread>
#include <unistd.h>
#include <string>

#include "../../../color.h"
using namespace ANSIColors;

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

void Client::receiveMessages(int sock) {
    char buffer[1024] = {0};
    while (running) {
        memset(buffer, 0, sizeof(buffer));
        int bytesRead = recv(sock, buffer, sizeof(buffer), 0);
        if (bytesRead <= 0) {
            std::cout << "\nServer closed the connection" << std::endl;
            running = false;
            break;
        }
        std::string message(buffer);
        if (message.find("SYSTEM:") != std::string::npos) {
            std::cout << "\r" << YELLOW << message << RESET << std::endl;
        } else {
            std::cout << "\r" << message << std::endl;
        }
        std::cout << "> ";
        std::cout.flush();
    }
}

bool Client::connectToServer(const char* serverIP, int port) {
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock < 0) {
        std::cerr << "Socket creation error" << std::endl;
        return false;
    }

    struct sockaddr_in serv_addr;
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

    if (strcmp(buffer, "100") == 0) {
        std::cout << "Enter your username: ";
        std::cin >> input;
        std::string user_set = "USER_SET " + input;
        send(sock, user_set.c_str(), user_set.length(), 0);
        
        // Clear buffer and read server response
        memset(buffer, 0, sizeof(buffer));
        valread = read(sock, buffer, 1024);

        // Check for USER_OK in the response
        if (strstr(buffer, "USER_OK") != nullptr) {
            std::cout << "User authentication successful" << std::endl;
        } else {
            // If USER_OK is not in the first message, wait for the next message
            memset(buffer, 0, sizeof(buffer));
            valread = read(sock, buffer, 1024);
            if (strstr(buffer, "USER_OK") != nullptr) {
                std::cout << "User authentication successful" << std::endl;
            } else {
                std::cerr << "User authentication failed" << std::endl;
                close(sock);
                return false;
            }
        }
    } else {
        std::cerr << "Unexpected server response" << std::endl;
        close(sock);
        return false;
    }

    running = true;
    receiveThread = std::thread(&Client::receiveMessages, this, sock);

    std::cout << "Connected to server. Type '/exit' to quit or 'CHAT [message]' to send a message." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    while (running) {
        std::cout << "> ";
        std::cout.flush();
        std::getline(std::cin, input);
        if (input == "/exit") {
            send(sock, "DISS", 4, 0);  // Send disconnect message to server
            break;
        }
        if (input.substr(0, 5) == "CHAT ") {
            send(sock, input.c_str(), input.length(), 0);
        } else if (!input.empty()) {
            std::cout << "Invalid command. Use 'CHAT [message]' to send a message." << std::endl;
        }
    }

    running = false;
    if (receiveThread.joinable()) {
        receiveThread.join();
    }
    close(sock);
    std::cout << "Disconnected from server" << std::endl;
    return true;
}