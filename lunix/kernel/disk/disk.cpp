#include "disk.h"
#include "../kernel/kernel.h"
#include <iostream>
#include <fstream>
#include <experimental/filesystem>

using namespace std;
using namespace std::experimental::filesystem;

namespace fs = std::experimental::filesystem;

kernel Kernel;

disk::disk() {}

void disk::rootfs() {
    string path = "rootfs";
    string usr_input;
    if (!exists(path)) {
        cout << "\nThe rootfs directory doesn't exist. Make new rootfs at " << current_path() << "? (y/n): ";
        cin >> usr_input;
        if (usr_input == "y" || usr_input == "Y") {
            cout << "Creating rootfs...";
            if (create_directory(path)) {
                cout << "done\n";
            } else {
                Kernel.panic("Failed to mount rootfs");
            }
        } else if (usr_input == "n" || usr_input == "N") {
            Kernel.panic("Failed to mount rootfs");
        } else {
            disk::rootfs();
        }
    } else if (exists(path)) {
        cout << "done\n";
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
