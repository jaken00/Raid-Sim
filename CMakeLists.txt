cmake_minimum_required(VERSION 3.25)

project(RaidSim
    VERSION 0.1.0
    DESCRIPTION "Football-Manager style raiding guild simulator"
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG        v1.91.0
)
FetchContent_MakeAvailable(imgui)

add_subdirectory(src)

option(RAIDSIM_BUILD_TESTS "Build test suite" ON)
if(RAIDSIM_BUILD_TESTS)
    enable_testing()
    add_subdirectory(test)
endif()
