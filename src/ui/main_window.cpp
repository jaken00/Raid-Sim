#include "main_window.h"

#include <imgui.h>

void DrawMainWindow(const EncounterResult& entry) {
    ImGui::Begin("Raid Manager");
    ImGui::Separator();

    // Boss summary
    ImGui::Text("Boss Summary");
    ImGui::Text("Duration: %.1f", entry.boss_hp_at_end);
    ImGui::Spacing();

    // Raid output
    ImGui::Text("Raid Output");
    ImGui::Separator();
    ImGui::Text("Duration: %.1f", entry.actual_duration);
    ImGui::Text("Total DPS: %.1f", entry.total_dps);
    

    ImGui::Spacing();

    // Deaths
    //ImGui::Text("Deaths: %d", (int)entry.deaths);
    ImGui::Spacing();

    // Roster


    ImGui::End();
}
