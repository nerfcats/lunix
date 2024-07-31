#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <unordered_map>

class Server {
public:
    Server();
    ~Server();

    void start();
    void stop();

private:
    void run();
    void handleClient(int clientSocket);
    void broadcastMessage(const std::string& message, int senderSocket);
    void broadcastSystemMessage(const std::string& message);
    void handleChatCommand(const std::string& command, int clientSocket);
    void closeAllConnections();

    std::thread serverThread;
    std::atomic<bool> running;
    const int PORT = 6942;
    std::vector<int> clientSockets;
    std::mutex clientMutex;
    std::unordered_map<int, std::string> clientUsernames;
};

#endif // SERVER_H