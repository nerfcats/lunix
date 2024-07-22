#include "disk.h"
#include "../kernel/kernel.h"
#include "../kernel/error_handler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>

using namespace std;
using namespace std::filesystem;

namespace fs = std::filesystem;

kernel Kernel;
extern error_handler ErrHandler;

disk::disk() {}

void disk::rootfs() {
    std::string path = "rootfs";
    std::string usr_input;

    if (!fs::exists(path)) {
        std::cout << "\nThe rootfs directory doesn't exist. Make new rootfs at " << fs::current_path() << "? (y/n): ";
        std::getline(std::cin, usr_input); // Use getline to capture input

        if (usr_input == "y" || usr_input == "Y") {
            std::cout << "Creating rootfs...";
            if (fs::create_directory(path)) {
                std::cout << "done\n";
            } else {
                ErrHandler.panic("Failed to mount rootfs");
            }
        } else if (usr_input == "n" || usr_input == "N") {
            ErrHandler.panic("Failed to mount rootfs");
        } else {
            std::cout << "Invalid input. Please enter 'y' or 'n'.\n";
            disk::rootfs(); // Retry if invalid input
        }
    } else {
        std::cout << "done\n";
    }
}


int disk::fopen(const std::string& filename, std::ios::openmode mode) {
    fs.open(filename, mode);
    if (fs.is_open()) {
        return 0;
    } else {
        return 1;
    }
}

int disk::fclose() {
    if (fs.is_open()) {
        fs.close();
        return 0;
    } else {
        return 1;
    }
}

int disk::fread(char* buffer, std::streamsize size) {
    if (fs.is_open() && fs.read(buffer, size)) {
        return static_cast<int>(fs.gcount());
    } else {
        return -1;
    }
}

int disk::fwrite(const char* buffer, std::streamsize size) {
    if (fs.is_open() && fs.write(buffer, size)) {
        return 0;
    } else {
        return 1;
    }
}

int disk::funlink(const std::string& filename) {
    if (remove(filename.c_str()) == 0) {
        return 0;
    } else {
        return 1;
    }
}

int disk::fopenbin(const std::string& binary) {
    pid_t pid = fork();  // Create a child process

    if (pid < 0) {
        // Fork failed
        std::cerr << "Fork failed" << std::endl;
        return -1;
    } else if (pid == 0) {
        // Child process
        char *args[] = {const_cast<char*>(binary.c_str()), nullptr};
        execvp(args[0], args);  // Replace the child process with the new binary

        // If execvp returns, it must have failed
        std::cerr << "Exec failed" << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0);  // Wait for the child process to finish

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);  // Return the exit status of the child process
        } else {
            return -1;  // Child process did not terminate normally
        }
    }
}

int disk::fmkdir(const std::string& path) {
    if (create_directory(path)) {
        return 0;
    } else {
        return 1;
    }
}

int disk::frmdir(const std::string& path) {
    if (remove(path.c_str()) == 0) {
        return 0;
    } else {
        return 1;
    }
}

int disk::frmdir_r(const std::string& path) {
    try {
        if (fs::exists(path) && fs::is_directory(path)) {
            // Recursively delete contents of the directory
            for (auto& entry : fs::directory_iterator(path)) {
                if (fs::is_directory(entry.status())) {
                    // Recur into subdirectory
                    if (frmdir_r(entry.path().string()) != 0) {
                        return 1;  // Return failure if any subdirectory deletion fails
                    }
                } else {
                    // Remove file
                    if (fs::remove(entry.path())) {
                        continue;
                    } else {
                        return 1;  // Return failure if file removal fails
                    }
                }
            }
            // Remove the directory itself
            if (fs::remove(path)) {
                return 0;  // Success
            } else {
                return 1;  // Failure to remove directory
            }
        } else {
            return 1;  // Path does not exist or is not a directory
        }
    } catch (const fs::filesystem_error& e) {
        cerr << "Filesystem error: " << e.what() << endl;
        return 1;  // Return failure in case of exceptions
    }
}

int disk::fchdir(const std::string& path) {
    if (exists(path) && is_directory(path)) {
        current_path(path);
        return 0;
    } else {
        return 1;
    }
}

string disk::fcwd() {
    return current_path().string();
}

void disk::umount() {
    for (auto* ifs : openInputFiles) {
        if (ifs->is_open()) {
            ifs->close();
        }
    }

    for (auto* ofs : openOutputFiles) {
        if (ofs->is_open()) {
            ofs->close();
        }
    }

    openInputFiles.clear();
    openOutputFiles.clear();
}

int disk::ftest() {
    for (int i = 0; i < 10000; i++) {
        if (disk::fmkdir("test_disk") != 0) {
            return 1;
        }
        if (disk::frmdir("test_disk") != 0) {
            return 1;
        }
    }
    return 0;
}
