#ifndef SERVER_H
#define SERVER_H

#include <thread>
#include <atomic>

class Server {
public:
    Server();
    ~Server();

    void start();
    void stop();

private:
    void run();
    std::thread serverThread;
    std::atomic<bool> running;
    const int PORT = 6942;
};

#endif // SERVER_H