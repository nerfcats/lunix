// <one line to give the program's name and a brief idea of what it does.>
// SPDX-FileCopyrightText: 2024 <copyright holder> <email>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef LSH_H
#define LSH_H

#include <iostream>

using namespace std;

/**
 * @todo write docs
 */
class lsh
{
public:
    lsh();
    int lshStart();
private:
    void printHelp();
    void man(const std::string& command);
    void changeDirectory(const std::string& path);
    void printWorkingDirectory();
    void catFile(const std::string& filename);
    void simpleEditor(const std::string& filename);
    void listFiles(const std::string& path = ".");
};

#endif // LSH_H
