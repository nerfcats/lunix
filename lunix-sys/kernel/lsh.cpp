// lsh.cpp
// SPDX-FileCopyrightText: 2024 <copyright holder> <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lsh.h"

#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>

#include "disk/disk.h"
#include "kernel/kernel.h"
#include "kernel/error_handler.h"
#include "color.h"
#include "security/userman.h"

using namespace ANSIColors;

namespace fs = std::filesystem;
extern disk Disk;
extern kernel Kernel;
extern error_handler ErrHandler;
UserManager userManager;

lsh::lsh() {}

void lsh::printHelp() {
    const std::vector<std::pair<std::string, std::string>> commands = {
        {"cat <file>", "Display the contents of a file"},
        {"cd <directory>", "Change the current working directory"},
        {"chmod <args>", "Change the permissions of a file or directory"},
        {"editor <file>", "Open a simple text editor (use 'nano' for more advanced features)"},
        {"exit", "Exit the shell"},
        {"help", "Display this help information"},
        {"ls", "List files and directories in the current directory"},
        {"mkdir <directory>", "Create a new directory in the current working folder"},
        {"nano", "Run the Nano text editor"},
        {"pwd", "Print the current working directory"},
        {"rl", "Display the current system runlevel"},
        {"rm [-R] <file/directory>", "Remove a file or empty directory\n"
                                     "                              Use -R to delete a directory and its contents recursively"},
        {"shutdown", "Shut down the system and exit the shell"},
        {"ver", "Display the OS and shell version information"}
    };

    std::cout << "\n" << "Available Commands:" << "\n\n";
    for (const auto& [cmd, desc] : commands) {
        std::cout << "  " << cmd << "\n    " << desc << "\n\n";
    }
}

void lsh::changeDirectory(const std::string& path) {
    Disk.fchdir(path);
}

void lsh::printWorkingDirectory() {
    std::cout << fs::current_path() << std::endl;
}

void lsh::catFile(const std::string& filename) {
    std::ifstream file(filename);
    if (file) {
        std::cout << file.rdbuf();
    } else {
        std::cerr << "No such file: " << filename << std::endl;
    }
}

void lsh::simpleEditor(const std::string& filename) {
    std::string content;
    std::cout << "Simple Editor\n";
    std::cout << "Editing " << filename << ". Type ':wq!' to save and exit.\n";

    std::ofstream outFile(filename, std::ios::app);
    while (true) {
        std::getline(std::cin, content);
        if (content == ":wq!") break;
        outFile << content << std::endl;
    }
    outFile.close();
}

void lsh::listFiles() {
    for (const auto& entry : fs::directory_iterator(fs::current_path())) {
        std::cout << entry.path().filename().string() << std::endl;
    }
}

/**
 * Starts the Lunix shell.
 *
 * This function initializes user management and handles the login process.
 * It displays a welcome message and a prompt for user input.
 * The function continuously reads user commands and executes them until the user enters "shutdown" or "exit".
 * Supported commands include changing directories, listing files, creating directories, viewing file contents,
 * editing files, changing permissions, deleting files and directories, and more.
 *
 * @return 0 upon successful completion.
 */
int lsh::lshStart() {
    userManager.initialize();  // Initialize user management and handle login

    Kernel.crlrq(4);
    std::string command;
    fs::path rootfsPath = fs::absolute("rootfs");  // Store the absolute path of rootfs

    std::cout << "\nWelcome to Lunix, a small simulation OS built on C++" << std::endl;
    std::cout << "lsh shell 0.1.0; type 'help' for commands\n\n";
    while (true) {
        fs::path currentPath = fs::current_path();
        std::string promptPath;

        if (currentPath == rootfsPath) {
            promptPath = "~";
        } else if (currentPath.string().find(rootfsPath.string()) == 0) {
            promptPath = "~" + currentPath.string().substr(rootfsPath.string().length());
        } else {
            promptPath = currentPath.string();
        }

        std::cout << (userManager.isRoot() ? RED + "root" + RESET : GREEN + userManager.getUsername() + RESET)
                  << "@" << CYAN << "lunix " << RESET << promptPath << " # ";

        if (!std::getline(std::cin, command)) {
            std::cout << std::endl;  // Print a newline to move to the next line after the prompt
            break;
        }

        if (command == "shutdown" || command == "exit") {
            break;
        } else if (command == "") {

        } else if (command == "help") {
            printHelp();
        } else if (command.substr(0, 3) == "cd ") {
            if (Disk.fchdir(command.substr(3)) != 0) {
                std::cout << "Directory " << command.substr(3) << " not found.\n";
            }
        } else if (command == "pwd") {
            std::cout << Disk.fcwd() << std::endl;
        } else if (command == "ls") {
            listFiles();
        } else if (command.substr(0, 6) == "mkdir ") {
            Disk.fmkdir(command.substr(6));
        } else if (command.substr(0, 4) == "cat ") {
            catFile(command.substr(4));
        } else if (command.substr(0, 7) == "editor ") {
            simpleEditor(command.substr(7));
        } else if (command.substr(0, 4) == "nano") {
            system(command.c_str());
        } else if (command.substr(0, 5) == "chmod") {
            system(command.c_str());
        } else if (command.substr(0, 2) == "rl") {
            std::cout << Kernel.runlevel << std::endl;
        } else if (command.substr(0, 3) == "rm ") {
            std::string args = command.substr(3);
            bool recursive = false;

            if (args.substr(0, 3) == "-R ") {
                recursive = true;
                args = args.substr(3);
            }

            if (recursive) {
                if (Disk.frmdir_r(args) == 0) {
                    std::cout << "Deleted directory " << args << " recursively.\n";
                } else {
                    std::cout << "Failed to delete directory " << args << ".\n";
                }
            } else {
                if (Disk.funlink(args) == 0) {
                    std::cout << "Deleted " << args << ".\n";
                } else {
                    std::cout << "Failed to unlink " << args << ". Error code " << Disk.funlink(args) << std::endl;
                }
            }
        } else if (command == "clear") {
            system("clear");
        } else if (command == "ver") {
            std::cout << Kernel.ver << std::endl;
        } else if (command.substr(0, 2) == "./") {
            std::string executable = command.substr(0);
            if (Disk.fopenbin(executable) != 0) {
                std::cout << "Failed to execute '" << executable << "'." << std::endl;
            }
        } else if (command == "panic") {
            if (userManager.isRoot()) {
                ErrHandler.panic("User initiated panic using 'panic' command");
            } else {
                cout << "Failed to run command: Permission denied\n";
            }
        } else if (command.substr(0, 6) == "passwd") {
            if (userManager.isRoot()) {
                std::string user, newPassword;
                std::istringstream iss(command.substr(7));
                iss >> user >> newPassword;
                userManager.setPassword(user, newPassword);
            } else {
                std::cerr << "Only root can change other users' passwords.\n";
            }
        } else if (command == "lulu") {
            std::cout << R"(  /^ ^\
 / 0 0 \
 V\ Y /V
  / - \
 /    |
V__) ||
)";
        } else {
            std::cout << "Command not found: " << command << std::endl;
        }
    }

    return 0;
}

