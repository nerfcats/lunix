#include "disk.h"
#include "../kernel/kernel.h"
#include "../kernel/error_handler.h"
#include "../security/userman.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <cstring>
#include <algorithm>
#include <vector>

using namespace std;
namespace fs = std::filesystem;

kernel Kernel;
extern error_handler ErrHandler;
extern UserManager userManager;

disk::disk() {}

void disk::rootfs() {
    std::string path = "rootfs";
    std::string pathmod = "modules";
    std::string usr_input;
    fs::path rootfsPath = fs::absolute(path);
    this->rootfsAbsolutePath = rootfsPath.string(); // Save the absolute path

    fs::path modPath = rootfsPath / pathmod; // Make modPath a subdirectory of rootfs

    if (!fs::exists(rootfsPath)) {
        std::cout << "\nThe rootfs directory doesn't exist. Make new rootfs at " << fs::current_path() << "? (y/n): ";
        std::getline(std::cin, usr_input);

        if (usr_input == "y" || usr_input == "Y") {
            std::cout << "Creating rootfs...";
            if (fs::create_directory(rootfsPath)) {
                std::cout << "done\n";
            } else {
                ErrHandler.panic("Failed to create rootfs");
            }
        } else if (usr_input == "n" || usr_input == "N") {
            ErrHandler.panic("Failed to mount rootfs");
        } else {
            std::cout << "Invalid input. Please enter 'y' or 'n'.\n";
            disk::rootfs(); // Retry if invalid input
            return;
        }
    } else {
        std::cout << "Rootfs exists.\n";
    }

    std::cout << "Changing to rootfs directory...\n";
    try {
        fs::current_path(rootfsPath);
        std::cout << "Current working directory: " << fs::current_path() << std::endl;
    } catch (const fs::filesystem_error& e) {
        ErrHandler.panic("Failed to change to rootfs directory: " + std::string(e.what()));
    }

    // Now check for the modules directory
    if (!fs::exists(modPath)) {
        std::cout << "Creating modules directory...";
        if (fs::create_directory(modPath)) {
            std::cout << "done\n";
        } else {
            ErrHandler.oops("Failed to create modules directory. To install modules you may have to create the folder manually.");
        }
    } else {
        std::cout << "Modules directory found; loading modules...\n";

        // Loop through each file in the modules directory
        for (const auto& entry : fs::directory_iterator(modPath)) {
            if (entry.is_regular_file() && entry.path().extension() == ".py") {
                std::cout << "Loaded " << entry.path().filename().string() << std::endl;
            }
        }
    }
}

int disk::loadMod(const std::string& modName) {
    // Always look for modules in the absolute rootfsAbsolutePath
    std::string modulesDir = "modules";
    fs::path modPath = fs::path(this->rootfsAbsolutePath) / modulesDir / (modName + ".py");

    // Check if the module exists and is a file
    if (!fs::exists(modPath) || !fs::is_regular_file(modPath)) {
        std::cerr << "Error: Module " << modName << " does not exist in " << modPath << std::endl;
        return -1;  // Error code for file not found
    }

    // Create a child process to run the Python script
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Error: Fork failed" << std::endl;
        return -1;  // Fork failed
    } else if (pid == 0) {
        // In child process, execute the python3 interpreter with the module
        std::string modPathStr = modPath.string();
        char* args[] = {const_cast<char*>("python3"), const_cast<char*>(modPathStr.c_str()), nullptr};

        execvp(args[0], args);  // Replace the child process with python3
        // If execvp returns, it means the exec call failed
        std::cerr << "Error: Exec failed for module " << modName << std::endl;
        exit(EXIT_FAILURE);  // Exit child process on failure
    } else {
        // In parent process, wait for the child to complete
        int status;
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            return WEXITSTATUS(status);  // Return the exit code of the Python script
        } else {
            std::cerr << "Error: Child process did not terminate normally" << std::endl;
            return -1;  // Return error if the process didn't terminate normally
        }
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
    // Check if the file is in the protected files list
    if (std::find(protectedFiles.begin(), protectedFiles.end(), filename) != protectedFiles.end()) {
        // If the file is protected, check if the user is root
        if (!userManager.isRoot()) {
            std::cerr << "Permission denied: " << filename << " is a protected file." << std::endl;
            return -1;  // Return -1 to indicate an error
        }
    }
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
    if (fs::create_directory(path)) {  // Use fs::create_directory
        return 0;
    } else {
        return 1;
    }
}

int disk::frmdir(const std::string& path) {
    if (fs::remove_all(path) > 0) {  // Use fs::remove_all
        return 0;
    } else {
        return 1;
    }
}

int disk::frmdir_r(const std::string& path) {
    if (fs::remove_all(path) > 0) {  // Use fs::remove_all
        return 0;
    } else {
        return 1;
    }
}

int disk::fchdir(const std::string& path) {
    if (chdir(path.c_str()) == 0) {
        return 0;
    } else {
        return 1;
    }
}

int disk::ftest() {
    std::cout << "done" << std::endl;
    return 0;
}

void disk::umount() {
    std::cout << "Unmounting..." << std::endl;
}

std::string disk::fcwd() {
    return fs::current_path().string();
}
