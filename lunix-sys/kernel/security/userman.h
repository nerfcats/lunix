// userman.h
#ifndef USERMAN_H
#define USERMAN_H

#include <string>

class UserManager {
public:
    UserManager();
    void initialize();
    bool login();
    void setPassword(const std::string& username, const std::string& password);
    std::string getUsername() const;
    bool isRoot() const;

private:
    std::string currentUsername;
    bool isRootUser;

    void createPasswordFile();
    bool passwordFileExists() const;
    bool authenticate(const std::string& username, const std::string& password);
    std::string hashPassword(const std::string& password);
};

#endif // USERMAN_H
