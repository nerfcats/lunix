#!/bin/bash

# Navigate to the build directory of Lunix
cd ../../lunix-sys/build

# Copy the lunix binary to the bootloader build directory
cp lunix ../../lunix-bl/build

# Navigate to the bootloader build directory
cd ../../lunix-bl/build
# Rename lunix to kernel.bin
mv lunix kernel.bin

# Generate the SHA-256 hash of kernel.bin and save it to kernel.bin.sha256
sha256sum kernel.bin > kernel.bin.sha256

# Get the current date and time
build_date=$(date +"%Y-%m-%d %H:%M:%S")

# Write the build date to .builddate file
echo "$build_date" > .builddate

echo "Post build script finished"
