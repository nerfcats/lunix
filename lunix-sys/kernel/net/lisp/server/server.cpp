#include "server.h"
#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <ifaddrs.h>
#include <arpa/inet.h>

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

std::string getPrivateIP() {
    struct ifaddrs *ifap, *ifa;
    struct sockaddr_in *sa;
    char *addr;

    if (getifaddrs(&ifap) != 0) {
        return "";
    }

    std::string privateIP = "";

    for (ifa = ifap; ifa; ifa = ifa->ifa_next) {
        if (ifa->ifa_addr && ifa->ifa_addr->sa_family == AF_INET) {
            sa = (struct sockaddr_in *) ifa->ifa_addr;
            addr = inet_ntoa(sa->sin_addr);
            std::string ipStr(addr);

            if (ipStr.find("192.168.") == 0 || ipStr.find("10.") == 0 ||
                (ipStr.find("172.") == 0 && stoi(ipStr.substr(4, 2)) >= 16 && stoi(ipStr.substr(4, 2)) <= 31)) {
                privateIP = ipStr;
                break;
            }
        }
    }

    freeifaddrs(ifap);
    return privateIP;
}

void Server::run() {
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket == -1) {
        std::cerr << "Error creating socket\n";
        return;
    }

    std::string privateIP = getPrivateIP();
    if (privateIP.empty()) {
        std::cerr << "Error retrieving private IP address\n";
        close(serverSocket);
        return;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(privateIP.c_str());
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

    std::cout << "\n\nLunix Inter-terminal Server Protocol" << std::endl;
    std::cout << "Server running on " << privateIP << ":" << PORT << std::endl;
    std::cout << "NOTE: ALWAYS type 'server stop' to stop the server! Otherwise ports or sockets can remain open and the server may have trouble starting again." << std::endl;

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
            std::cout << "Client sent: " << buffer << std::endl;

            const char* response = "pong";
            send(clientSocket, response, strlen(response), 0);

            close(clientSocket);
        }
    }

    // Close port and socket before continuing
    shutdown(serverSocket, SHUT_RDWR);
    close(serverSocket);
}
