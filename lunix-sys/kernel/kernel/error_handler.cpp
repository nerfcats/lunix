#include "error_handler.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <unistd.h> // for geteuid
#include <execinfo.h>
#include <csignal> // for signal handling
#include <sstream> // for std::stringstream
#include <cstdlib>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include "kernel.h"
#include "../color.h"
#include "../net/network.h"
#include "../disk/disk.h"
#include "../lsh.h"

using namespace std;
using namespace std::filesystem;
using namespace ANSIColors;

int oops_count = 0;
bool restart_after_panic = false;

extern kernel Kernel;

error_handler::error_handler() {}

void error_handler::panic(string reason) {
    // Capture panic information
    std::stringstream panic_info;
    panic_info << BG_RED << BOLD_WHITE << "\nKernel panic: " << reason << endl;
    panic_info << "Stack trace:\n";

    const int maxFrames = 64;
    void* buffer[maxFrames];
    int numFrames = backtrace(buffer, maxFrames);
    char** symbols = backtrace_symbols(buffer, numFrames);

    if (symbols) {
        for (int i = 0; i < numFrames; ++i) {
            panic_info << "  " << symbols[i] << endl;
        }
        free(symbols);
    } else {
        panic_info << "Failed to retrieve stack trace symbols." << endl;
    }

    // Add more debugging information
    panic_info << "System has panicked...\n";
    panic_info << "OS Version: " << Kernel.ver << endl; // Replace with actual version
    panic_info << "CPU Architecture: " << "x86_64" << endl; // Replace with actual

    panic_info << "\nend Kernel panic: " << reason << RESET << endl;

    // Output the panic information
    std::cout << panic_info.str();

    // Log to file
    log_to_file(panic_info.str());

    if (restart_after_panic) {
        system("../lunix_bootloader -r");
        exit(1);
    } else {
        // Hang the system infinitely
        while (true) {this_thread::sleep_for(chrono::seconds(1));}
    }
}

void error_handler::oops(string reason) {
    oops_count += 1;
    // Capture panic information
    std::stringstream panic_info;
    panic_info << BG_YELLOW << BOLD_WHITE << "\nKernel oops: " << reason << endl;
    panic_info << "Stack trace:\n";

    const int maxFrames = 64;
    void* buffer[maxFrames];
    int numFrames = backtrace(buffer, maxFrames);
    char** symbols = backtrace_symbols(buffer, numFrames);

    if (symbols) {
        for (int i = 0; i < numFrames; ++i) {
            panic_info << "  " << symbols[i] << endl;
        }
        free(symbols);
    } else {
        panic_info << "Failed to retrieve stack trace symbols." << endl;
    }

    // Add more debugging information
    panic_info << "System is in an UNSTABLE state...\n";
    panic_info << "OS Version: " << Kernel.ver << endl; // Replace with actual version
    panic_info << "CPU Architecture: " << "x86_64" << endl; // Replace with actual

    panic_info << "\nend Kernel oops: " << reason << RESET << endl;

    // Output the panic information
    std::cout << panic_info.str();

    // Log to file
    log_to_file(panic_info.str());

    if (oops_count >= 10) {
        std::ostringstream oss;
        oss << "too many oopses: " << reason;
        panic(oss.str());
    }

}

void error_handler::print_stack_trace() {
    const int maxFrames = 64;
    void* buffer[maxFrames];
    int numFrames = backtrace(buffer, maxFrames);
    char** symbols = backtrace_symbols(buffer, numFrames);

    if (symbols) {
        for (int i = 0; i < numFrames; ++i) {
            std::cout << "  " << symbols[i] << std::endl;
        }
        free(symbols);
    } else {
        std::cout << "Failed to retrieve stack trace symbols." << std::endl;
    }
}

void error_handler::setup_signal_handlers() {
    signal(SIGSEGV, error_handler::handle_signal); // Segmentation fault
    signal(SIGABRT, error_handler::handle_signal); // Abort signal from abort()
    signal(SIGFPE, error_handler::handle_signal);  // Floating-point exception
    signal(SIGILL, error_handler::handle_signal);  // Illegal instruction
    signal(SIGBUS, error_handler::handle_signal);  // Bus error
}

void error_handler::handle_signal(int signal) {
    string signal_name;
    switch (signal) {
        case SIGSEGV: signal_name = "Segmentation fault"; break;
        case SIGABRT: signal_name = "Fatal exception"; break;
        case SIGFPE: signal_name = "Floating-point exception"; break;
        case SIGILL: signal_name = "Illegal instruction"; break;
        case SIGBUS: signal_name = "Bus error"; break;
        default: signal_name = "Unknown signal recieved"; break;
    }
    if (signal_name != "Unknown signal") {
        error_handler().panic(signal_name);
    } else {
        error_handler().oops(signal_name);
    }
}

void error_handler::log_to_file(const string& log_message) {
    ofstream log_file("kernel_panic.log", ios_base::app);
    if (log_file.is_open()) {
        log_file << log_message;
        log_file.close();
    } else {
        std::cout << "Failed to open log file." << std::endl;
    }
}
