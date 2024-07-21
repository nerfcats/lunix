#ifndef DISK_H
#define DISK_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

class disk
{
public:
    disk();

    // Configure root filesystem folder
    void rootfs();

    // File operations
    std::fstream fs;
    int fopen(const std::string& filename, std::ios::openmode mode);
    int fclose();
    int fread(char* buffer, std::streamsize size);
    int fwrite(const char* buffer, std::streamsize size);
    int funlink(const std::string& filename);

    // Directory operations
    int fmkdir(const std::string& path);
    int frmdir(const std::string& path);
    int frmdir_r(const std::string& path);
    int fchdir(const std::string& path);

    int ftest();

    void umount();
    std::string fcwd();

private:
    std::vector<std::ifstream*> openInputFiles;
    std::vector<std::ofstream*> openOutputFiles;
};

#endif // DISK_H
