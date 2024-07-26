# Internal Scructure
Updated as of 7/25/24

Lunix Project Structure:
- lunix (root directory)
  - lunix-sys (kernel directory)
    - kernel (main kernel directory)
      - kernel (bare kernel)
      - disk (disk operations)
      - net (networking)
      - lsh.cpp & lsh.h (lunix shell [lsh])
      - color.h (text formatting)
    - lunix-bl (bootloader directory)

To unlock the bootloader, run it manually with `--ignore-hash-err` flag.

Each part of the kernel that handles something is separated into two files:
- .h (function declarations)
- .cpp (function definitions)

Example:
example.h
```cpp
int someFunction();
```

example.cpp
```cpp
int someFunction() {
  doSomething();
  return 0;
}
```

