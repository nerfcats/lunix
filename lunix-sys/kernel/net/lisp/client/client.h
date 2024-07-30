#ifndef CLIENT_H
#define CLIENT_H

class Client {
public:
    Client();
    bool pingServer(const char* serverIP, int port);
};

#endif // CLIENT_H