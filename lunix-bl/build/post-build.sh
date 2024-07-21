#!/bin/bash

# Navigate to the build directory of Lunix project
cd ../../lunix/build

# Copy the lunix binary to the bootloader build directory
cp lunix ../../lunix-bl/build

# Navigate to the bootloader build directory
cd ../../lunix-bl/build
# Rename lunix to kernel.bin
mv lunix kernel.bin

# Generate the SHA-256 hash of kernel.bin and save it to kernel.bin.sha256
sha256sum kernel.bin > kernel.bin.sha256

echo "Post build script finished"
