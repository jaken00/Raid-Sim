#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <imgui.h>
#include <imgui_impl_opengl3.h>
#include <imgui_impl_sdl2.h>

#include <algorithm>
#include <iostream>

#include "data/seeder.h"
#include "db/database.h"
#include "ui/main_window.h"
#include "core/gamestate.h"

int main(int argc, char* argv[]) {
    Database db("raid.db");
    if (!db.init())
        return 1;
    Seeder::seed(db);

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL Init failed: " << SDL_GetError() << "\n";
        return 1;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_FLAGS, 0);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    SDL_Window* window =
        SDL_CreateWindow("Raid Sim", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 1280, 720,
                         SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << "\n";
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init("#version 130");

    GameState gameState;
    gameState.runLoader(db);

    // Simulation state
    bool               sim_ready     = false;
    bool               run_sim       = false;
    EncounterResult    result{};
    std::vector<FightStep> sim_history;
    int                current_step  = 0;
    float              step_accum    = 0.0f;
    float              playback_speed = 10.0f;
    Uint32             last_ticks    = SDL_GetTicks();

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT)
                running = false;
        }

        // Delta time
        Uint32 now   = SDL_GetTicks();
        float  delta = (now - last_ticks) / 1000.0f;
        last_ticks   = now;

        // Trigger simulation when button was pressed
        if (run_sim) {
            result       = gameState.attemptRaid();
            sim_history  = gameState.getSimHistory();
            current_step = 0;
            step_accum   = 0.0f;
            sim_ready    = true;
            run_sim      = false;
        }

        // Advance playback
        if (sim_ready && current_step < (int)sim_history.size() - 1) {
            step_accum += delta * playback_speed;
            int advance = (int)step_accum;
            step_accum -= (float)advance;
            current_step = std::min(current_step + advance, (int)sim_history.size() - 1);
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        DrawMainWindow(result, sim_ready, run_sim);
        DrawDebugWindow(sim_history, current_step, playback_speed);

        ImGui::Render();
        glViewport(0, 0, 1280, 720);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
