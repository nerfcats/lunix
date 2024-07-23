<img src="https://github.com/noahdossan/Lunix/blob/59b9e7d90a7bdcbc3a6119e00a31a14058005577/images/logo.png" alt="Lunix logo" title="Lunix" align="right" height="60" />

# Lunix

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](https://makeapullrequest.com) 
[![kernel build](https://github.com/noahdossan/Lunix/actions/workflows/kernel-build.yml/badge.svg)](https://github.com/noahdossan/Lunix/actions/workflows/kernel-build.yml)

Lunix (Lulu's UNIX, Lulu being the name of my pup :3) is a very cool simulation hobby OS written in C++ that runs in your terminal. It is a UNIX-clone and has several features such as file editing, kernel panics, "libraries" to handle stuff like Disk and Networking, error handling, sudo protection and some other stuff. It can also run compiled binaries and bash scripts using `./program-name`.

Please ⭐ the project if you think it's cool. It helps me stay motivated and continue to develop Lunix :).

## Table of Contents
- [Project Direction 🚀](#project-direction)
- [Screenshots 🖼️](#screenshot)
- [Build Instructions 🔨](#build-instructions)
  - [Requirements](#requirements)
  - [Automatic Build](#automatic-build)
  - [Manual Build](#manual-build)
- [Running Lunix 🏃](#running-lunix)
- [Documentation 📖](#documentation)
- [Contribution 🏗️](#contribution)
- [App Development 💾](#app-development)

## Project Direction
I plan to make Lunix basically a full simulation of a UNIX/Linux terminal OS. I might (somehow) add a desktop enviroment.
Also, when the project starts to mature a little, I might start using Releases so no building is needed.

## Screenshot

![Screenshot 1: Demo GIF of terminal](https://github.com/noahdossan/Lunix/blob/9752eb99282886595c5743513ea651e4653bfe66/images/demo.gif)

## Build Instructions
To run Lunix you need to build both the kernel and the bootloader.

> [!IMPORTANT]
> Lunix can't be built on Windows, as it uses UNIX only libraries. Please use WSL or a Linux VM.

### Requirements
To build Lunix, you need:
- `cmake`
- `make`
- `gcc`
- `g++`
- `libssl-dev/openssl-devel`

To install these packages on Ubuntu/Debian run `sudo apt install cmake make gcc g++ libssl-dev`.
To install these packages on Fedora run `sudo dnf install cmake make gcc g++ openssl-devel`.

Run `git clone https://github.com/noahdossan/lunix.git` and `cd lunix`.

### Automatic Build
In the main Lunix directory, type `./Build.sh` to build Lunix if you haven't built it yet. To rebuild type `./Build.sh --rebuild`.

### Manual Build
You have to build the kernel and bootloader in order, since the bootloaders post build script needs the kernel to be built.

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
In the main directory where you cloned Lunix, run the Run.sh script with `./Run.sh`

To run Lunix manually, go to the bootloader directory (lunix-bl) and type `./lunix-bl`. If it returns a file missing error run `./lunix-bl -b` to rerun the post build script.

## Documentation
Working on that.

## Contribution
Contributions are very welcome :D! I am still new to this GitHub thing so I haven't made guidelines or stuff like that yet.

## App Development
For now Lunix can only run compiled binaries and bash scripts externally, and C++ kernel code internally. I might add a Lunix scripting language that it can parse and run.

![Dog ASCII art](https://github.com/noahdossan/Lunix/blob/1fe0a5969c9768406357046517e46fa3fc49fccf/lulu-terminal.png)
