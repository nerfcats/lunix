cmake_minimum_required(VERSION 3.0)

project(lunix-bl)

find_package(OpenSSL REQUIRED)
include_directories(${OPENSSL_INCLUDE_DIR})

add_executable(lunix-bl main.cpp)

target_link_libraries(lunix-bl OpenSSL::SSL)

install(TARGETS lunix-bl RUNTIME DESTINATION bin)

add_custom_command(TARGET lunix-bl
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E echo "Running post-build script..."
    COMMAND ${CMAKE_COMMAND} -E env bash -c "bash -x post-build.sh"
    COMMAND ${CMAKE_COMMAND} -E echo "Post-build script done."
)
