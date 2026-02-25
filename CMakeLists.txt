cmake_minimum_required(VERSION 3.20)
project(RaidSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(SQLite3 REQUIRED)

add_executable(RaidSim src/main.cpp src/db/database.cpp src/data/seeder.cpp)

target_include_directories(RaidSim PRIVATE ${SQLite3_INCLUDE_DIRS})
target_link_libraries(RaidSim SDL2::SDL2main SDL2::SDL2 ${SQLite3_LIBRARIES})