// network.cpp; Networking driver
// SPDX-FileCopyrightText: 2024 Noah Dos Santos noahpds@proton.me
// SPDX-License-Identifier: GPL-3.0-or-later

#include "network.h"
#include <iostream>

network::network() {}

int network::test() {
    std::cout << "         Testing network... (Ctrl+C to cancel)\n";
    int result = std::system("ping -c 1 -W 3 8.8.8.8 > /dev/null 2>&1");
    if (result != 0) {
        return 1;
    }
    return 0;
}
