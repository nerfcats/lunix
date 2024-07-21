cmake_minimum_required(VERSION 3.0)

project(lunix)

# Add executable sources
add_executable(lunix
    main.cpp
    kernel/kernel/kernel.cpp
    kernel/net/network.cpp
    kernel/disk/disk.cpp
    kernel/lsh.cpp
    kernel/windowserver/windowserver.cpp
)

target_link_libraries(lunix stdc++fs)

# Find SDL2
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Find SDL_ttf
find_package(SDL2_ttf REQUIRED)
if(NOT SDL2_TTF_FOUND)
    # Manually set the SDL2_ttf paths if not found
    set(SDL2_TTF_LIBRARIES /usr/lib/x86_64-linux-gnu/libSDL2_ttf.so)
    set(SDL2_TTF_INCLUDE_DIRS /usr/include/SDL2)
endif()
include_directories(${SDL2_TTF_INCLUDE_DIRS})

# Link SDL2 and SDL_ttf
target_link_libraries(lunix ${SDL2_LIBRARIES} ${SDL2_TTF_LIBRARIES})

# Specify installation target
install(TARGETS lunix RUNTIME DESTINATION bin)
