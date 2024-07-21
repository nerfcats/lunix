cmake_minimum_required(VERSION 3.0)

project(lunix)

# Add executable sources
add_executable(lunix
    main.cpp
    kernel/kernel/kernel.cpp
    kernel/kernel/error_handler.cpp
    kernel/net/network.cpp
    kernel/disk/disk.cpp
    kernel/lsh.cpp
)

target_link_libraries(lunix stdc++fs)

# Specify installation target
install(TARGETS lunix RUNTIME DESTINATION bin)
