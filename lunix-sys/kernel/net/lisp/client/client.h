#ifndef CLIENT_H
#define CLIENT_H

#include <atomic>
#include <thread>

class Client {
public:
    Client();
    bool pingServer(const char* serverIP, int port);
    bool connectToServer(const char* serverIP, int port);
private:
    void receiveMessages(int sock);
    std::atomic<bool> running;
    std::thread receiveThread;
};

#endif // CLIENT_H