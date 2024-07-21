#include "kernel.h"
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
#include "../color.h"
#include "../net/network.h"
#include "../disk/disk.h"
#include "../lsh.h"
#include "error_handler.h"

using namespace std;
using namespace std::filesystem;
using namespace ANSIColors;

error_handler ErrHandler;

network Network;
disk Disk;
lsh LSH;

kernel::kernel() {
    ErrHandler.setup_signal_handlers();
}

void kernel::halt(string reason) {
    cout << endl << "System cannot continue: " << reason << " Halted.\n";
    exit(0);
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
        ErrHandler.panic("Ran as user ID 0 (root)!");
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
