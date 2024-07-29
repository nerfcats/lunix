// userman.cpp
#include "userman.h"

#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

static void disableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag &= ~ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

static void enableEcho() {
    struct termios tty;
    tcgetattr(STDIN_FILENO, &tty);
    tty.c_lflag |= ECHO;
    tcsetattr(STDIN_FILENO, TCSANOW, &tty);
}

std::string getPassword() {
    disableEcho();
    std::string password;
    std::getline(std::cin, password);
    enableEcho();
    std::cout << std::endl;
    return password;
}

UserManager::UserManager() : currentUsername(""), isRootUser(false) {}

void UserManager::initialize() {
    if (!passwordFileExists()) {
        createPasswordFile();
    }
    if (!login()) {
        std::cerr << "Failed to login. Exiting...\n";
        exit(1);
    }
}

bool UserManager::login() {
    std::string username, password;

    std::cout << "Username: ";
    std::cin >> username;
    std::cout << "Password: ";
    std::cin.ignore();  // Ignore leftover newline character from std::cin
    password = getPassword();

    if (authenticate(username, password)) {
        currentUsername = username;
        isRootUser = (username == "root");
        return true;
    }

    std::cerr << "Invalid username or password.\n";
    return false;
}

void UserManager::setPassword(const std::string& username, const std::string& password) {
    std::ofstream passwordFile(".passwd", std::ios::app);
    if (passwordFile.is_open()) {
        passwordFile << username << "." << hashPassword(password) << "\n";
        passwordFile.close();
    } else {
        std::cerr << "Unable to open password file.\n";
    }
}

std::string UserManager::getUsername() const {
    return currentUsername;
}

bool UserManager::isRoot() const {
    return isRootUser;
}

void UserManager::createPasswordFile() {
    std::ofstream passwordFile(".passwd");
    if (passwordFile.is_open()) {
        std::string rootPassword;
        std::cout << "No password file found. Create a password for root: ";
        rootPassword = getPassword();
        passwordFile << "root." << hashPassword(rootPassword) << "\n";
        passwordFile.close();
    } else {
        std::cerr << "Unable to create password file.\n";
    }
}

bool UserManager::authenticate(const std::string& username, const std::string& password) {
    std::ifstream passwordFile(".passwd");
    std::string line;

    if (passwordFile.is_open()) {
        std::string user, hash;
        std::string passwordHash = hashPassword(password);

        while (getline(passwordFile, line)) {
            std::istringstream iss(line);
            getline(iss, user, '.');
            getline(iss, hash);

            if (user == username && hash == passwordHash) {
                passwordFile.close();
                return true;
            }
        }
        passwordFile.close();
    }

    return false;
}

std::string UserManager::hashPassword(const std::string& password) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256(reinterpret_cast<const unsigned char*>(password.c_str()), password.length(), hash);

    std::ostringstream result;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        result << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }

    return result.str();
}

bool UserManager::passwordFileExists() const {
    return fs::exists(".passwd");
}
