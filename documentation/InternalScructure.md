# Internal Scructure
Updated as of 7/22/24

Lunix is seperated to two main folders:
- lunix/lunix (the kernel)
- lunix/lunix-bl (the bootloader)

The bootloader checks the hash of the kernel against the stored hash, then boots the kernel. To unlock the bootloader run it manually with `--ignore-hash-err`.

## Kernel Structure
The kernel has different folders which correspond to their contents:
```
- lunix/lunix/kernel/ (main kernel directory)
- kernel/kernel (the bare kernel)
- kernel/disk (part that handles disk operations)
- kernel/net (part that handles networking)
- kernel/lsh.cpp & lsh.h (lunix shell [lsh])
- kernel/color.h (text formatting)
```
Each part of the kernel that handles something are seperated in two files
- .h (function declarations)
- .cpp (function definitions)
```
example.h
int someFunction();

example.cpp
int someFunction() {
  doSomething();
  return 0;
}
```
