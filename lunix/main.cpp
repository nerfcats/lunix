// main.cpp; Starts the kernel
// SPDX-FileCopyrightText: 2024 Noah Dos Santos noahpds@proton.me
// SPDX-License-Identifier: GPL-3.0-or-later

/*
 * Lunix (Lulu's Unix) is an umbrella term for all things related such as but not limited to:
 * LUNAR kernel
 * lsh (Lunix shell)
 */

#include <iostream>
#include <string>

#include "kernel/kernel/kernel.h"

int main(int argc, char **argv) {
    // Start the kernel
    std::cout << "Loading the kernel...\n";
    kernel Kernel;
    Kernel.start();

    return 0;
}
