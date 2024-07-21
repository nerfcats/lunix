# Lunix
Lunix is a very cool simulation hobby OS written in C++
## Build Instructions
To run Lunix you need to build both the kernel and the bootloader.

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

## How to run
Go to the bootloader directory (lunix-bl) and type `./lunix-bl`. If it returns a file missing error run `./lunix-bl -b` to rerun the post build script.
