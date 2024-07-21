<img src="https://github.com/noahdossan/Lunix/blob/59b9e7d90a7bdcbc3a6119e00a31a14058005577/images/logo.png" alt="Lunix logo" title="Lunix" align="right" height="60" />

# Lunix

[![CMake on a single platform](https://github.com/noahdossan/Lunix/actions/workflows/cmake-single-platform.yml/badge.svg)](https://github.com/noahdossan/Lunix/actions/workflows/cmake-single-platform.yml)

Lunix (Lulu's UNIX, Lulu being the name of my pup :3) is a very cool simulation hobby OS written in C++. It is a UNIX-clone and has several features such as file editing, kernel panics and "libraries" to handle stuff like Disk and Networking.

Please ⭐ the project if you think it's cool. It helps me stay motivated and continue to develop Lunix :)

## Table of Contents
- [Project Direction 🚀](#project-direction)
- [Screenshots 🖼️](#screenshots)
- [Build Instructions 🔨](#build-instructions)
  - [Automatic Build](#automatic-build)
  - [Manual Build](#manual-build)
- [Running Lunix 🏃](#running-lunix)

## Project Direction
I plan to make Lunix basically a full simulation of a UNIX/Linux terminal OS. I might (somehow) add a desktop enviroment.
Also, when the project starts to mature a little, I might start using Releases so no building is needed.

## Screenshots
![Screenshot 1: Dog ASCII art](https://github.com/noahdossan/Lunix/blob/1fe0a5969c9768406357046517e46fa3fc49fccf/lulu-terminal.png)

![Screenshot 2: Demo GIF of terminal](https://github.com/noahdossan/Lunix/blob/9752eb99282886595c5743513ea651e4653bfe66/images/demo.gif)

![Screenshot 3: Overall terminal](https://github.com/noahdossan/Lunix/blob/1fe0a5969c9768406357046517e46fa3fc49fccf/lunix-terminal.png)

## Build Instructions
To run Lunix you need to build both the kernel and the bootloader.

### Requirements
To build Lunix, you need:
- CMake
- Make
- GCC

To install these packages run `sudo apt install cmake make gcc`

### Automatic Build
I've made a script to do this automatically, in the main Lunix directory, type `./Build.sh` to build Lunix for first time. To rebuild type `./Build.sh --rebuild`.

### Manual Build
You have to build the kernel and bootloader in order. Kernel first then bootloader second

To build the kernel:
```
cd lunix
mkdir build
cd build
cmake ..
make -j$(nproc)
```
To build the bootloader:
```
cd ../../lunix-bl # Assuming you were in the lunix directory
cd build
cmake ..
make -j$(nproc)
```
Once you build the bootloader it will run a `post-build.sh` script. If it returns an error make sure you built the lunix kernel properly.

## Running Lunix
Go to the bootloader directory (lunix-bl) and type `./lunix-bl`. If it returns a file missing error run `./lunix-bl -b` to rerun the post build script.
