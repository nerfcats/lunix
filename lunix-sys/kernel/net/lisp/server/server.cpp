#include "server.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

Server::Server() : running(false) {}

Server::~Server() {
    if (running) {
        stop();
    }
}

void Server::start() {
    if (!running) {
        running = true;
        serverThread = std::thread(&Server::run, this);
    }
}

void Server::stop() {
    if (running) {
        running = false;
        if (serverThread.joinable()) {
            serverThread.join();
        }
    }
}

void Server::run() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(PORT);

    if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) < 0) {
        std::cerr << "Error binding socket\n";
        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 3) < 0) {
        std::cerr << "Error listening on socket\n";
        close(serverSocket);
        return;
    }

    std::cout << "Server listening on port " << PORT << std::endl;

    while (running) {
        fd_set readfds;
        FD_ZERO(&readfds);
        FD_SET(serverSocket, &readfds);

        struct timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        int activity = select(serverSocket + 1, &readfds, NULL, NULL, &timeout);

        if (activity < 0) {
            std::cerr << "Select error\n";
            break;
        }

        if (activity == 0) {
            continue;  // Timeout, check running flag
        }

        if (FD_ISSET(serverSocket, &readfds)) {
            int clientSocket = accept(serverSocket, NULL, NULL);
            if (clientSocket < 0) {
                std::cerr << "Error accepting connection\n";
                continue;
            }

            char buffer[1024] = {0};
            read(clientSocket, buffer, 1024);
            std::cout << "Received: " << buffer << std::endl;

            const char* response = "Pong";
            send(clientSocket, response, strlen(response), 0);

            close(clientSocket);
        }
    }

    // Close port and socket before continuing
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
}