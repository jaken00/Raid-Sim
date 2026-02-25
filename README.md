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
cd /c/dev/raid-manager -- Work computer
cd /c/programming/dev/Raid-Sim/ -- Home PC
cd build
cmake ..
mingw32-make
```

cmake .. — This is the configure step. It reads your CMakeLists.txt, finds your libraries (SDL2, SQLite3), and
  generates the actual build files (Makefiles or Visual Studio project files). It does not compile any of your code.

  cmake --build . — This is the compile step. It takes those generated build files and actually compiles your .cpp files
   into an executable. This is what you're missing — until you run this, RaidSim.exe is still the old version.

  .\RaidSim.exe — This runs the compiled program.

  So the full workflow every time you change code is:
  cmake --build .       ← compile
  .\RaidSim.exe         ← run

  You only need to re-run cmake .. if you change CMakeLists.txt (adding files, libraries, etc.). For normal code
  changes, cmake --build . is all you need.


## Run

From the `build/` directory:

```bash
./RaidSim.exe
```

If you get an error about `SDL2.dll` not found, copy it next to the executable:

```bash
cp /ucrt64/bin/SDL2.dll .
```
