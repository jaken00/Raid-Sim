#include "main_window.h"

#include <algorithm>
#include <cstdio>
#include <imgui.h>

void DrawMainWindow(const EncounterResult& entry, bool sim_ready, bool& run_sim, bool& restart_sim) {
    ImGui::Begin("Raid Manager");
    ImGui::Separator();

    if (ImGui::Button("Run Simulation")) {
        run_sim = true;
    }

    ImGui::Spacing();

    if (sim_ready) {
        bool boss_killed = entry.boss_hp_at_end <= 0.0f;

        if (boss_killed) {
            ImGui::TextColored(ImVec4(0.2f, 1.0f, 0.2f, 1.0f), "BOSS KILLED");
        } else {
            ImGui::TextColored(ImVec4(1.0f, 0.3f, 0.3f, 1.0f), "WIPE");
        }

        ImGui::Spacing();
        ImGui::Text("Boss HP at end: %.0f", entry.boss_hp_at_end);
        ImGui::Separator();
        ImGui::Text("Duration:   %.1f s", entry.actual_duration);
        ImGui::Text("Total DPS:  %.1f",   entry.total_dps);
        ImGui::Text("Survived:   %d",     entry.survived);

        ImGui::Spacing();
        ImGui::Separator();

        if (ImGui::Button("Restart Fight")) {
            restart_sim = true;
        }

        if (boss_killed) {
            ImGui::SameLine();
            if (ImGui::Button("Next Phase")) {
                restart_sim = true;
            }
        }
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

    for (const auto& name : step.deaths_this_step) {
        ImGui::TextColored(ImVec4(1.0f, 0.2f, 0.2f, 1.0f), "DEATH: %s", name.c_str());
    }

    ImGui::Separator();

    ImGui::Text("Current DPS: %.1f", step.fight_state.current_dps);
    ImGui::Text("Current HPS: %.1f", step.fight_state.current_hps);

    ImGui::Separator();

    // Per-player DPS debug table
    if (ImGui::BeginTable("PlayerDebug", 8,
            ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg | ImGuiTableFlags_ScrollY,
            ImVec2(0.0f, 150.0f))) {
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

    // Unit frames — compact WoW-style raid boxes
    ImGui::Text("Unit Frames");
    ImGui::Spacing();

    if (!step.player_health.empty()) {
        const float FRAME_W = 110.0f;
        const float FRAME_H = 42.0f;
        const float GAP     = 4.0f;

        float avail = ImGui::GetContentRegionAvail().x;
        int   cols  = std::max(1, (int)((avail + GAP) / (FRAME_W + GAP)));

        ImDrawList* dl = ImGui::GetWindowDrawList();

        for (int i = 0; i < (int)step.player_health.size(); i++) {
            if (i % cols != 0)
                ImGui::SameLine(0.0f, GAP);

            const auto& snap = step.player_health[i];
            float pct = (snap.max_hp > 0.0f)
                ? std::clamp(snap.current_hp / snap.max_hp, 0.0f, 1.0f)
                : 0.0f;

            ImVec2 p = ImGui::GetCursorScreenPos();
            ImGui::Dummy(ImVec2(FRAME_W, FRAME_H));

            // Dark background
            dl->AddRectFilled(p, {p.x + FRAME_W, p.y + FRAME_H},
                              IM_COL32(28, 28, 28, 255), 3.0f);

            // Health fill — WoW-style color thresholds
            ImU32 fill_col;
            if (pct > 0.75f)      fill_col = IM_COL32(30,  160,  30,  210);
            else if (pct > 0.35f) fill_col = IM_COL32(200, 175,  20,  210);
            else if (pct > 0.0f)  fill_col = IM_COL32(200,  35,  35,  210);
            else                  fill_col = IM_COL32(55,   55,  55,  210);

            dl->AddRectFilled(p, {p.x + FRAME_W * pct, p.y + FRAME_H},
                              fill_col, 3.0f);

            // Border
            dl->AddRect(p, {p.x + FRAME_W, p.y + FRAME_H},
                        IM_COL32(90, 90, 90, 220), 3.0f);

            // Player name (top-left)
            dl->AddText({p.x + 4.0f, p.y + 4.0f},
                        IM_COL32(255, 255, 255, 220), snap.name.c_str());

            // HP% (bottom-left)
            char hp_buf[16];
            snprintf(hp_buf, sizeof(hp_buf), "%.0f%%", pct * 100.0f);
            dl->AddText({p.x + 4.0f, p.y + FRAME_H - 16.0f},
                        IM_COL32(210, 210, 210, 200), hp_buf);
        }
    } else {
        ImGui::TextDisabled("No player health data.");
    }

    ImGui::Spacing();
    ImGui::Separator();

    // Playback speed slider
    ImGui::SliderFloat("Steps/sec", &playback_speed, 1.0f, 120.0f);

    ImGui::End();
}
