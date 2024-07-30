![Lunix Logo](https://github.com/noahdossan/lunix/blob/main/images/lunix-logo-lowres.png)

[![PRs Welcome](https://img.shields.io/badge/PRs-welcome-brightgreen.svg?style=flat-square)](https://makeapullrequest.com) 
[![kernel build](https://github.com/noahdossan/Lunix/actions/workflows/kernel-build.yml/badge.svg)](https://github.com/noahdossan/Lunix/actions/workflows/kernel-build.yml)

Lunix (Lulu's UNIX, named after my pupper Lulu) is a hobby operating system written in C++ that simulates a UNIX-like environment in your terminal. It features:

- **File Editing**: Create and modify files directly within Lunix.
- **Libraries**: Built-in "libraries" for handling disk operations, networking, and more.
- **Error Handling**: Robust error handling mechanisms.
- **Multiple Users**: Add other users without root perms. Passwords are stored as a sha256 hash.
- **Executable Support**: Run compiled binaries and bash scripts using `./program-name`.
- **Kernel Panics**: Kernel panic events that trigger on actual signals such as SIGABRT.
- and much more!

If you find Lunix interesting, please ⭐ the project. Your support keeps me motivated to continue developing and improving Lunix!

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
Lunix aims to be a comprehensive simulation of a UNIX/Linux terminal operating system. The project's primary focus is on adding new features and commands to enhance the user experience. Future plans include the development of custom scripts specifically for Lunix, providing users with even more functionality. The current development efforts are concentrated on expanding the feature set and improving the overall performance and stability of Lunix.

## Screenshot

![Screenshot 1: Demo GIF of terminal](https://github.com/noahdossan/Lunix/blob/9752eb99282886595c5743513ea651e4653bfe66/images/demo.gif)

## Build Instructions

To run Lunix, you need to build both the kernel and the bootloader.

> [!IMPORTANT]
> Lunix can't be built on Windows, as it uses UNIX-only libraries. Please use WSL or a Linux VM.

### Requirements

To build Lunix, you need the following packages installed:

- `cmake`
- `make`
- `gcc`
- `g++`
- `libssl-dev/openssl-devel`

To install these packages on Ubuntu/Debian, run the following command:
```
sudo apt install cmake make gcc g++ libssl-dev
```

To install these packages on Fedora, run the following command:
```
sudo dnf install cmake make gcc g++ openssl-devel
```

After installing the required packages, clone the Lunix repository and navigate to the cloned directory:
```
git clone https://github.com/noahdossan/lunix.git
cd lunix
```

### Automatic Build

In the main Lunix directory, run the following command to build Lunix:
```
./Build.sh -nb
```
If you have already built Lunix and want to rebuild modified source files, use the `-b` flag:
```
./Build.sh -b
```

### Manual Build

To build the kernel, follow these steps:

1. Navigate to the Lunix directory:
```
cd lunix
```

2. Create a build directory:
```
mkdir build
cd build
```

3. Generate the build files using CMake:
```
cmake ..
```

4. Build the kernel using `make`:
```
make -j$(nproc)
```

To build the bootloader, follow these steps:

1. Navigate to the bootloader directory:
```
cd ../../lunix-bl
cd build
```

2. Generate the build files using CMake:
```
cmake ..
```

3. Build the bootloader using `make`:
```
make -j$(nproc)
```

Once you have built the bootloader, it will run a `post-build.sh` script. If you encounter any errors, make sure you have built the Lunix kernel properly.

## Running Lunix

To run Lunix, follow these steps:

1. In the main directory where you cloned Lunix, run the `Run.sh` script by executing `./Run.sh`.

2. If you prefer to run Lunix manually, navigate to the bootloader directory (`lunix-bl`) and execute `./lunix-bl`. If you encounter a file missing error, you can rerun the post build script by executing `./lunix-bl -b`.


## Documentation

The documentation for Lunix is currently a work in progress. We welcome contributions to help improve and expand the documentation.

To access the documentation, please visit the [Documentation](/docs) page.


## Contribution
Contributors are very welcome to Lunix :D! Please read the [Contribution Guidelines](/CONTRIBUTING.md) first.
