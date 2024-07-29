#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <cstring>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <iomanip> // For setw and setfill
#include <openssl/evp.h>
#include "color.h"

using namespace ANSIColors;

// Function to compute SHA-256 hash of a file using the EVP API
std::string computeSHA256(const std::string &filePath) {
    unsigned char hash[EVP_MAX_MD_SIZE];
    unsigned int lengthOfHash = 0;
    char buf[1024];
    EVP_MD_CTX *mdctx;

    if ((mdctx = EVP_MD_CTX_new()) == nullptr) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }
    if (1 != EVP_DigestInit_ex(mdctx, EVP_sha256(), nullptr)) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to initialize digest context");
    }

    std::ifstream file(filePath, std::ifstream::binary);
    if (!file) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Could not open file: " + filePath);
    }
    while (file.good()) {
        file.read(buf, sizeof(buf));
        if (1 != EVP_DigestUpdate(mdctx, buf, file.gcount())) {
            EVP_MD_CTX_free(mdctx);
            throw std::runtime_error("Failed to update digest");
        }
    }

    if (1 != EVP_DigestFinal_ex(mdctx, hash, &lengthOfHash)) {
        EVP_MD_CTX_free(mdctx);
        throw std::runtime_error("Failed to finalize digest");
    }
    EVP_MD_CTX_free(mdctx);

    std::stringstream ss;
    for (unsigned int i = 0; i < lengthOfHash; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(hash[i]);
    }
    return ss.str();
}

// Function to read the expected SHA-256 hash from a file
std::string readSHA256File(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file) {
        throw std::runtime_error("Could not open file: " + filePath);
    }
    std::string hash;
    file >> hash;
    return hash;
}

int main(int argc, char* argv[]) {
    bool ignoreHashError = false;
    bool runPostBuild = false;
    bool recoveryMode = false;
    bool helpMode = false;
    std::string bMode = "BOOT";

    // Check for command-line arguments
    for (int i = 1; i < argc; ++i) {
        if (std::strcmp(argv[i], "--ignore-hash-err") == 0) {
            ignoreHashError = true;
        } else if (std::strcmp(argv[i], "-b") == 0) {
            runPostBuild = true;
        } else if (std::strcmp(argv[i], "-r") == 0) {
            recoveryMode = true;
            bMode = "RECOVERY";
        } else if (std::strcmp(argv[i], "-h") == 0) {
            helpMode = true;
            bMode = "HELP";
        }
    }

    std::cout << BOLD_WHITE << "LUNIX BOOTLOADER (" << bMode << ")" << RESET << "\n\n";

    if (runPostBuild) {
        // Execute post-build.sh
        std::cout << "Running post build script" << std::endl;
        system("./post-build.sh");
    } else if (recoveryMode) {
        system("clear");
        std::cout << BOLD_WHITE << "LUNIX BOOTLOADER (RECOVERY)" << RESET << "\n\n";
        std::cout << YELLOW << "The system has crashed due to a panic or fatal error and rebooted. Kernel panic logs are stored in the rootfs folder.\n";
        std::cout << "To start Lunix again, run ./lunix_bootloader\n\n" << RESET;
        exit(0);
    } else if (helpMode) {
        system("clear");
        std::cout << BOLD_WHITE << "LUNIX BOOTLOADER" << RESET << "\n\n";
        std::cout << "Bootloader arguments\n";
        std::cout << "  -b          Run post build script\n";
        std::cout << "  -r          Boot into recovery mode\n";
        std::cout << "  -h --help   Show this help menu\n";
        exit(0);
    }

    try {
        std::string computedHash = computeSHA256("kernel.bin");
        std::string expectedHash = readSHA256File("kernel.bin.sha256");

        if (computedHash != expectedHash) {
            if (ignoreHashError) {
                std::cerr << BOLD_YELLOW << "WARNING: HASH MISMATCH! The kernel is either CORRUPTED or possibly TAMPERED with.\nYour machine is now at risk." << RESET << std::endl;
            } else {
                std::cerr << BOLD_RED << "HASH MISMATCH! The kernel is either CORRUPTED or possibly TAMPERED with.\nTo unlock the bootloader and continue anyways, run the bootloader with --ignore-hash-err" << RESET << std::endl;
                return 1;
            }
        } else {
            std::cerr << BOLD_GREEN << "Kernel hash check passed!" << RESET << std::endl;
        }
    } catch (const std::exception &e) {
        std::cerr << BOLD_RED << "Error: " << e.what() << RESET << std::endl;
        return 1;
    }

    pid_t pid = fork();

    if (pid < 0) {
        // Fork failed
        std::cerr << "Fork failed" << std::endl;
        return 1;
    } else if (pid == 0) {
        // Child process
        const char *args[] = {"./kernel.bin", nullptr}; // Program to execute in current directory
        if (execvp(args[0], const_cast<char* const*>(args)) < 0) {
            std::cout << BOLD_YELLOW << "Lunix Recovery\n" << RESET;
            std::cout << BOLD_YELLOW << "Use keyboard to input and [ENTER].\n" << RESET;
            std::cout << CYAN << "---------------------------------------------------------------------------" << RESET << std::endl;
            std::cerr << BOLD_RED << "Failed to execute kernel: " << strerror(errno) << RESET << std::endl << "Make sure the kernel is in the same directory as the bootloader" << std::endl;

            // Ask user for the correct path
            std::string path;
            std::cout << "Path to kernel: ";
            std::cin >> path;

            // Convert user input to char array
            const char *userArgs[] = {path.c_str(), nullptr};
            if (execvp(userArgs[0], const_cast<char* const*>(userArgs)) < 0) {
                std::cerr << "Execution failed again: " << strerror(errno) << std::endl;
                std::cout << CYAN << "---------------------------------------------------------------------------" << RESET << std::endl;
                return 1;
            }
        }
    } else {
        // Parent process
        int status;
        waitpid(pid, &status, 0); // Wait for the child process to complete
        if (WIFEXITED(status)) {
            std::cout << "Child exited with status: " << WEXITSTATUS(status) << std::endl;
        } else {
            std::cerr << "Child process did not exit successfully" << std::endl;
        }
    }

    return 0;
}
