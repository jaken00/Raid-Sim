#include "main_window.h"

#include <imgui.h>

void DrawMainWindow(const PhaseResult& entry) {
    ImGui::Begin("Raid Manager");

    // Result banner
    if (entry.deaths == 0) {
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "RESULT: SUCCESS");
    } else {
        ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "RESULT: WIPE");
    }

    ImGui::Separator();

    // Boss summary

    ImGui::Spacing();

    // Raid output
    ImGui::Text("Raid Output");
    ImGui::Separator();
    ImGui::Text("Duration: %.1f", entry.actual_duration);
    ImGui::Text("Total DPS: %.1f", entry.total_dps);

    ImGui::Spacing();

    // Deaths
    ImGui::Text("Deaths: %d", (int)entry.deaths);
    ImGui::Spacing();

    // Roster


    ImGui::End();
}
