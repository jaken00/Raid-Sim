cmake_minimum_required(VERSION 3.20)
project(RaidSim)

set(CMAKE_CXX_STANDARD 17)

find_package(SDL2 REQUIRED)

add_executable(RaidSim src/main.cpp)

target_link_libraries(RaidSim SDL2::SDL2main SDL2::SDL2)