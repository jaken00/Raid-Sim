cmake_minimum_required(VERSION 3.20)
project(RaidSim)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(SDL2 REQUIRED)
find_package(SQLite3 REQUIRED)
find_package(OpenGL REQUIRED)

# Fetch ImGui
include(FetchContent)
FetchContent_Declare(
    imgui
    GIT_REPOSITORY https://github.com/ocornut/imgui.git
    GIT_TAG v1.91.0
)
FetchContent_MakeAvailable(imgui)

set(IMGUI_SOURCES
    ${imgui_SOURCE_DIR}/imgui.cpp
    ${imgui_SOURCE_DIR}/imgui_draw.cpp
    ${imgui_SOURCE_DIR}/imgui_tables.cpp
    ${imgui_SOURCE_DIR}/imgui_widgets.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_sdl2.cpp
    ${imgui_SOURCE_DIR}/backends/imgui_impl_opengl3.cpp
)

add_executable(RaidSim
    src/main.cpp
    src/db/database.cpp
    src/data/seeder.cpp
    ${IMGUI_SOURCES}
)

target_include_directories(RaidSim PRIVATE
    ${SQLite3_INCLUDE_DIRS}
    ${imgui_SOURCE_DIR}
    ${imgui_SOURCE_DIR}/backends
)

target_link_libraries(RaidSim
    SDL2::SDL2main
    SDL2::SDL2
    ${SQLite3_LIBRARIES}
    OpenGL::GL
)
