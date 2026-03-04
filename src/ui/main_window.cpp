#include "main_window.h"

#include <imgui.h>

void DrawMainWindow(Database& db) {
    ImGui::Begin("Raid Manager");

    PlayerRow player;
    BossRow boss;

    bool hasPlayer = db.getFirstPlayer(player);
    bool hasBoss = db.getFirstBoss(boss);

    if (hasPlayer) {
        ImGui::Text("Player");
        ImGui::Separator();
        ImGui::Text("Name: %s", player.name.c_str());
        ImGui::Text("Class: %s", player.class_name.c_str());
        ImGui::Text("Spec: %s", player.spec.c_str());
        ImGui::Text("Item Level: %.1f", player.ilvl);
        ImGui::Text("Level: %d", player.level);
    } else {
        ImGui::Text("No players found in database.");
    }

    ImGui::Separator();

    if (hasBoss) {
        ImGui::Text("Boss");
        ImGui::Separator();
        ImGui::Text("Name: %s", boss.name.c_str());
        ImGui::Text("Tuning iLvl: %d", boss.tuning_ilvl);
        ImGui::Text("HPS Threshold: %d", boss.hps_threshold);
        ImGui::Text("DPS Threshold: %d", boss.dps_threshold);
    } else {
        ImGui::Text("No bosses found in database.");
    }

    ImGui::End();
}

