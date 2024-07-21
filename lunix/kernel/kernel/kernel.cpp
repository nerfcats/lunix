#include "kernel.h"
#include <iostream>
#include <fstream>
#include <experimental/filesystem> // KDevelop does not like filesystem only
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
#include "../color.h"
#include "../net/network.h"
#include "../disk/disk.h"
#include "../lsh.h"
#include "../windowserver/windowserver.h"

using namespace std;
using namespace std::experimental::filesystem;
using namespace ANSIColors;

int oops_count = 0;
bool restart_after_panic = false;

network Network;
disk Disk;
lsh LSH;

kernel::kernel() {
    setup_signal_handlers();
}

void kernel::halt(string reason) {
    cout << endl << "System cannot continue: " << reason << " Halted.\n";
    exit(0);
}

void kernel::print_stack_trace() {
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

void kernel::log_to_file(const string& log_message) {
    ofstream log_file("kernel_panic.log", ios_base::app);
    if (log_file.is_open()) {
        log_file << log_message;
        log_file.close();
    } else {
        std::cout << "Failed to open log file." << std::endl;
    }
}

void kernel::panic(string reason) {
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
    panic_info << "OS Version: " << kernel::ver << endl; // Replace with actual version
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

void kernel::oops(string reason) {
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
    panic_info << "OS Version: " << kernel::ver << endl; // Replace with actual version
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

void kernel::haltrq(string reason) {
    kernel::halt(reason);
}

void kernel::crl(int rl) {
    runlevel = rl;
    cout << "Runlevel changed to " << to_string(runlevel) << endl;
}

void kernel::crlrq(int rl) {
    kernel::crl(rl);
}

void kernel::check_sudo() {
    if (geteuid() == 0) {
        kernel::panic("Ran as user ID 0 (root)!");
        exit(0);
    }
}

std::string dexec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


void kernel::start() {
    //system("clear");
    std::cout << RESET;

    std::cout << "\nStarting the Lunar kernel...\n";
    kernel::crl(1);
    kernel::check_sudo();
    std::cout << "An initial boot image isn't configured.\n";
    std::cout << "Mounting root filesystem..." << std::flush;

    Disk.rootfs();

    try {
        // List USB devices
        std::string usbDevices = dexec("lsusb");
        // List PCI devices
        std::string pciDevices = dexec("lspci");
        // List block devices
        std::string blockDevices = dexec("lsblk");
        // List devices in /dev
        //std::string devDevices = dexec("ls /dev");

        // Concatenate and print results
        std::string result = "Connected USB devices: " + usbDevices + " Connected PCI devices: " + pciDevices +
                             " Connected Block devices: " + blockDevices; // + " Devices in /dev: " + devDevices
        std::cout << result << std::endl;
    } catch (const std::exception &e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }

    kernel::crl(2);
    std::cout << std::endl;
    // Additional startup stuff
    if (Network.test() == 0) {
        std::cout << "[  " << GREEN << "OK" << RESET << "  ] " << "Started and tested network\n";
    } else {
        std::cout << "[" << RED << "FAILED" << RESET << "] " << "Failed to test network\n";
    }
    cout << "         Testing disk reliability...\n";
    if (Disk.ftest() == 0) {
        std::cout << "[  " << GREEN << "OK" << RESET << "  ] " << "Disk test PASS\n";
    } else {
        std::cout << "[" << RED << "FAILED" << RESET << "] " << "Disk test FAIL, currect directory might be write protected\n";
    }
    std::cout << std::endl;
    kernel::crl(3);
    std::cout << "Started the " << CYAN << "Lunix" << RESET << " OS successfully. Welcome!\n";
    LSH.lshStart();
    kernel::shutdown();
}

void kernel::shutdown() {
    cout << YELLOW << "\nSystem is going down NOW!\n" << RESET;
    cout << "Sending shutdown signals to all processes...\n";
    cout << "Unmounting all filesystems..." << flush;
    Disk.umount();
    cout << "done\n";
    cout << "Spinning down disks...\n";
    crl(0);
    cout << CYAN << "It is now safe to turn off your computer.\n\n" << RESET;

    exit(0);
}

void kernel::setup_signal_handlers() {
    signal(SIGSEGV, kernel::handle_signal); // Segmentation fault
    signal(SIGABRT, kernel::handle_signal); // Abort signal from abort()
    signal(SIGFPE, kernel::handle_signal);  // Floating-point exception
    signal(SIGILL, kernel::handle_signal);  // Illegal instruction
    signal(SIGBUS, kernel::handle_signal);  // Bus error
}

void kernel::handle_signal(int signal) {
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
        kernel().panic(signal_name);
    } else {
        kernel().oops(signal_name);
    }
}
