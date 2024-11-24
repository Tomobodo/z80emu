# z80emu

A naive attempt at emulating a z80 based on the user manual in doc.

## Cloning

```shell
git clone git@github.com:Tomobodo/z80emu.git
```

## Building

You'll need conan and cmake to build the project.
I wanted to try using a dependencies manager instead of just using submodules
but it does nothing to make pulling and building dependencies easier so might
revert back to just using git submodules at some point.

It has been developped and testes with llvm_clang on windows.

Not tested on other platforms or compilers.

### Requirements

- cmake > 3.30
- clang > 18.0
- conan > 2.9

### Build steps

1. Pull dependencies

Be sure you have set up conan properly and have profiles defined.

If you're new to conan you can generate initial profile using

```shell
conan profile detect --force
```

It will generate your default profile here : ```~/.conan2/profiles/default```.
Check it and edit it to fit your actual building configuration if needed.

```shell
conan install . --build=missing --profile=name_of_your_profile # do not set --profile to use your default one
```

2. Cmake build

```shell
cmake --preset conan-debug # use conan-release for release
cmake --build --preset conan-debug
```

3. Running

```shell
./build/Debug/z80emu
```
