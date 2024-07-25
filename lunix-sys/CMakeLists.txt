cmake_minimum_required(VERSION 3.0)

project(lunix)

# Find OpenSSL
find_package(OpenSSL REQUIRED)

# Add executable sources
add_executable(lunix
    main.cpp
    kernel/kernel/kernel.cpp
    kernel/kernel/error_handler.cpp
    kernel/net/network.cpp
    kernel/disk/disk.cpp
    kernel/lsh.cpp
    kernel/security/userman.cpp
)

# Link OpenSSL and filesystem libraries
target_link_libraries(lunix OpenSSL::SSL OpenSSL::Crypto stdc++fs)

# Specify installation target
install(TARGETS lunix RUNTIME DESTINATION bin)
