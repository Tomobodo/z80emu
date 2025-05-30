# z80emu

A naive attempt at emulating a z80 based on the user manual in doc.

## Cloning

```shell
git clone git@github.com:Tomobodo/z80emu.git
```

## Building

The project uses CMake FetchContent to automatically download and build dependencies.
This simplifies the build process significantly compared to the previous Conan setup.

It has been developed and tested with Clang on Windows.

Not tested on other platforms or compilers yet.

### Requirements

- cmake > 3.25
- A C++23 compatible compiler (MSVC 2022, Clang 15+, or GCC 12+)
- Git (for FetchContent to download dependencies)

### Build steps

1. Configure the project

CMake will automatically download SDL3 and ImGui using FetchContent:

```shell
cmake -B build
```

2. Build the project

```shell
cmake --build build
```

3. Running

```shell
./build/Debug/z80emu.exe path/to/binary
```

### Dependencies

The following dependencies are automatically managed by CMake FetchContent:

- **SDL3** (3.2.14): Cross-platform multimedia library
- **ImGui** (1.91.9b): Immediate mode GUI library for the debug interface
- **doctest** (2.4.11): unit testing

No manual dependency installation is required.
