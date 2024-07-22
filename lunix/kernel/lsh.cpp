// lsh.cpp
// SPDX-FileCopyrightText: 2024 <copyright holder> <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#include "lsh.h"

#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <cstdlib>

#include "disk/disk.h"
#include "kernel/kernel.h"
#include "color.h"

using namespace ANSIColors;

namespace fs = std::filesystem;
extern disk Disk;
extern kernel Kernel;

lsh::lsh() {}

void lsh::printHelp() {
    std::cout << "Commands:\n"
              << "  cd <directory>    - Change directory\n"
              << "  pwd               - Print working directory\n"
              << "  cat <file>        - Print file content\n"
              << "  ls                - List files and directories\n"
              << "  shutdown          - Exit the shell\n"
              << "  exit              - Exit the shell\n"
              << "  editor <file>     - Open simple text editor\n"
              << "  help              - Display this help\n"
              << "  rl                - Display current runlevel\n"
              << "  rm [-R] <file>    - Remove file or empty folder\n"
              << "                      Use -R to delete directory recursively\n"
              << "  mkdir <directory> - Make a directory in current working folder\n"
              << "  ver               - Display OS and shell version\n";
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

int lsh::lshStart() {
    Kernel.crlrq(4);
    std::string command;
    fs::path rootfsPath = fs::absolute("rootfs");  // Store the absolute path of rootfs
    Disk.fchdir("rootfs");
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

        std::cout << RED << "root" << RESET << "@" << CYAN << "lunix " << RESET << promptPath << " # ";
        if (!std::getline(std::cin, command)) {
            // Handle EOF (Ctrl-D)
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
            cout << Kernel.ver << endl;
        } else if (command.substr(0, 2) == "./") {
            std::string executable = command.substr(0);
            if (Disk.fopenbin(executable) != 0) {
                std::cout << "Failed to execute '" << executable << "'." << std::endl;
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
