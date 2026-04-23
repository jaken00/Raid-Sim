# RaidSim

A C++ project using SDL2, built with CMake and MSYS2 UCRT64.

## Prerequisites

1. Install [MSYS2](https://www.msys2.org/)
2. Open the **MSYS2 UCRT64** shell and install the required packages:

```bash
pacman -S mingw-w64-ucrt-x86_64-gcc mingw-w64-ucrt-x86_64-cmake mingw-w64-ucrt-x86_64-SDL2 mingw-w64-ucrt-x86_64-sqlite3
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





● Clean. Here's a summary of everything that changed and what you need to do to build.

  ---
  What was written/changed:

  ┌────────────────────────────────────────────────┬─────────────────────────────────────────────────────────────────┐
  │                      File                      │                             Action                              │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ CMakeLists.txt                                 │ Rewritten — minimal orchestrator, delegates to src/ and test/   │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ src/CMakeLists.txt                             │ New — raidsim_core static lib + raidsim exe                     │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ test/CMakeLists.txt                            │ New — doctest target linked against raidsim_core                │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ test/test_fight.cpp                            │ New — 4 cases on ilvl_factor                                    │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ vcpkg.json                                     │ New — sdl2, sqlite3, nlohmann-json, imgui[sdl2+opengl3],        │
  │                                                │ doctest                                                         │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ CMakePresets.json                              │ New — debug/release presets via $VCPKG_ROOT toolchain           │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ .gitignore                                     │ Expanded — covers CMake artifacts, .NET bin/obj, clangd cache,  │
  │                                                │ vcpkg_installed                                                 │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ src/sim/fight.h                                │ ilvl_factor removed from private class, re-declared as free     │
  │                                                │ function                                                        │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ src/sim/fight.cpp                              │ Fight::ilvl_factor → free function ilvl_factor                  │
  ├────────────────────────────────────────────────┼─────────────────────────────────────────────────────────────────┤
  │ tools/RaidSimTools/Services/JsonDataService.cs │ Hardcoded path replaced with RAIDSIM_DATA_DIR env var fallback  │
  └────────────────────────────────────────────────┴─────────────────────────────────────────────────────────────────┘

  ---
  Before you configure, run these once:

  # 1. Untrack committed build artifacts (safe — only removes from git index)
  git rm -r --cached tools/RaidSimTools/bin/ tools/RaidSimTools/obj/ a.out
  git add .gitignore
  git commit -m "clean: remove committed artifacts, expand .gitignore"

  # 2. Install vcpkg if you haven't (one-time)
  git clone https://github.com/microsoft/vcpkg.git C:/vcpkg
  C:/vcpkg/bootstrap-vcpkg.bat

  # 3. Set env var (add to your system env or MSYS2 profile permanently)
  export VCPKG_ROOT=/c/vcpkg

  Then build:

  cmake --preset debug
  cmake --build --preset debug
  ctest --preset debug

  One heads-up: the first cmake --preset debug triggers vcpkg installing all deps from source (SDL2, SQLite3, imgui,
  etc.). On Windows this takes 15–30 minutes. It's a one-time cost — results go to vcpkg_installed/ which is gitignored
  and cached on subsequent runs.

  Run those three build commands and paste back whatever error you hit first. The most likely friction points are vcpkg
  not finding a dep, or a missing include in one of the deeper headers.

✻ Cooked for 6m 49s











