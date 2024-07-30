#!/bin/bash

# Function to perform the default action
newBuild() {
  cd lunix-sys
  mkdir build
  cd build
  cmake ..
  make -j$(nproc)
  cd ../../lunix-bl/build
  cmake ..
  make -j$(nproc)
  ./post-build.sh
  echo "Done. To run check for build errors then run ./Run.sh"
}

# Function to perform the rebuild action
build() {
  cd lunix-sys/build
  make -j$(nproc)
  cd ../../lunix-bl/build
  make -j$(nproc)
}

# Check if any arguments are passed
if [ "$#" -eq 0 ]; then
  # No arguments passed
  echo "Usage: $0 [-nb] [-b]"
elif [ "$1" == "-nb" ]; then
  # -nb argument passed
  newBuild
elif [ "$1" == "-b" ]; then
  # -b argument passed
  build
else
  echo "Unknown argument: $1"
  echo "Usage: $0 [-nb] [-b]"
fi
