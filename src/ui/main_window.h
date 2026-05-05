#pragma once
#include "../sim/SimTypes.h"

void DrawMainWindow(const EncounterResult& entry, bool sim_ready, bool& run_sim, bool& restart_sim);
void DrawDebugWindow(const std::vector<FightStep>& history, int current_step, float& playback_speed);
