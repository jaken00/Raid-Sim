find_package(SDL2 CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
find_package(imgui CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(unofficial-sqlite3 CONFIG REQUIRED)

# Simulation core — no UI/SDL2/OpenGL; this is what tests link against
add_library(raidsim_core STATIC
    characters/Spec.cpp
    characters/player.cpp
    core/gamestate.cpp
    data/loader.cpp
    data/seeder.cpp
    db/database.cpp
    Raid/Boss.cpp
    sim/fight.cpp
)

target_include_directories(raidsim_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# TODO: replace with runtime path resolution
target_compile_definitions(raidsim_core PRIVATE
    DATA_DIR="${CMAKE_SOURCE_DIR}/src/data"
)

target_link_libraries(raidsim_core
  PUBLIC
    nlohmann_json::nlohmann_json
  PRIVATE
    unofficial::sqlite3::sqlite3
)

# Thin executable — only main + UI; all logic lives in raidsim_core
add_executable(raidsim
    main.cpp
    ui/main_window.cpp
)

target_link_libraries(raidsim
  PRIVATE
    raidsim_core
    SDL2::SDL2
    SDL2::SDL2main
    OpenGL::GL
    imgui::imgui
)
