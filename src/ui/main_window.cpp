#include "main_window.h"

#include <algorithm>
#include <imgui.h>

void DrawMainWindow(const EncounterResult& entry, bool sim_ready, bool& run_sim) {
    ImGui::Begin("Raid Manager");
    ImGui::Separator();

    if (ImGui::Button("Run Simulation")) {
        run_sim = true;
    }

    ImGui::Spacing();

    if (sim_ready) {
        ImGui::Text("Boss Summary");
        ImGui::Text("Boss HP at end: %.0f", entry.boss_hp_at_end);
        ImGui::Spacing();

        ImGui::Text("Raid Output");
        ImGui::Separator();
        ImGui::Text("Duration: %.1f s", entry.actual_duration);
        ImGui::Text("Total DPS: %.1f", entry.total_dps);
        ImGui::Text("Survived: %d", entry.survived);
    } else {
        ImGui::TextDisabled("Press 'Run Simulation' to begin.");
    }

    ImGui::End();
}

void DrawDebugWindow(const std::vector<FightStep>& history, int current_step, float& playback_speed) {
    ImGui::Begin("Fight Debug");

    if (history.empty()) {
        ImGui::TextDisabled("No simulation data yet.");
        ImGui::End();
        return;
    }

    int clamped = std::clamp(current_step, 0, (int)history.size() - 1);
    const FightStep& step = history[clamped];

    // Boss HP progress bar
    float hp_pct = std::clamp(step.boss_hp_pct, 0.0f, 1.0f);
    ImGui::Text("Boss HP: %.0f  (%.1f%%)", step.boss_hp_remaining, hp_pct * 100.0f);
    ImGui::ProgressBar(hp_pct, ImVec2(-1.0f, 0.0f));

    ImGui::Spacing();

    // Step and spell info
    ImGui::Text("Step: %d / %d", step.spells_resolved, step.total_spells);
    ImGui::Text("Spell: %s  dmg=%.0f  %s",
                step.spell_name.c_str(),
                step.damage_value,
                step.is_aoe ? "[AoE]" : "[ST]");

    // Deaths at this step
    for (const auto& name : step.deaths_this_step) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "DEATH: %s", name.c_str());
    }

    ImGui::Separator();

    // FightState
    ImGui::Text("Current DPS: %.1f", step.fight_state.current_dps);
    ImGui::Text("Current HPS: %.1f", step.fight_state.current_hps);

    ImGui::Separator();

    // Per-player FightDebugData table
    if (ImGui::BeginTable("PlayerDebug", 8,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
            ImVec2(0.0f, 160.0f))) {
        ImGui::TableSetupScrollFreeze(0, 1);
        ImGui::TableSetupColumn("Player");
        ImGui::TableSetupColumn("Perf");
        ImGui::TableSetupColumn("ilvl fac");
        ImGui::TableSetupColumn("Crit");
        ImGui::TableSetupColumn("Haste");
        ImGui::TableSetupColumn("Resist");
        ImGui::TableSetupColumn("Affinity");
        ImGui::TableSetupColumn("DPS");
        ImGui::TableHeadersRow();

        for (const auto& [name, dbg] : step.player_debug) {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("%s", name.c_str());
            ImGui::TableSetColumnIndex(1); ImGui::Text("%.0f", dbg.player_performance_rating);
            ImGui::TableSetColumnIndex(2); ImGui::Text("%.2f", dbg.ilvl_calculation);
            ImGui::TableSetColumnIndex(3); ImGui::Text("%.2f", dbg.crit_multiplier);
            ImGui::TableSetColumnIndex(4); ImGui::Text("%.2f", dbg.haste_multiplier);
            ImGui::TableSetColumnIndex(5); ImGui::Text("%.2f", dbg.boss_resist);
            ImGui::TableSetColumnIndex(6); ImGui::Text("%.2f", dbg.fight_affinity);
            ImGui::TableSetColumnIndex(7); ImGui::Text("%.0f", dbg.player_dps);
        }
        ImGui::EndTable();
    }

    ImGui::Separator();

    // Playback speed slider
    ImGui::SliderFloat("Steps/sec", &playback_speed, 1.0f, 120.0f);

    ImGui::End();
}
