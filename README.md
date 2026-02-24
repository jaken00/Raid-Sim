# RaidSim

A C++ project using SDL2, built with CMake and MSYS2 UCRT64.

## Prerequisites

1. Install [MSYS2](https://www.msys2.org/)
2. Open the **MSYS2 UCRT64** shell and install the required packages:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-SDL2
```

## Build

Open the **MSYS2 UCRT64** shell and run:

```bash
cd /c/dev/raid-manager
mkdir build
cd build
cmake ..
mingw32-make
```

## Run

From the `build/` directory:

```bash
./RaidSim.exe
```

If you get an error about `SDL2.dll` not found, copy it next to the executable:

```bash
cp /ucrt64/bin/SDL2.dll .
```
