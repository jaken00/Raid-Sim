#include "fight.h"

Fight::Fight(const std::vector<Player*>& players, Boss* boss) : players(players), boss(boss) {}

Fight::~Fight() {}

RaidEntry Fight::attemptBoss() {
    int avg_ilvl;
    std::vector<std::string> modifers;  // need to change this to an actual modifer class or enum?

    float dps_power = 0;
    float hps_power = 0;
    float defense_power = 0;

    float raid_dps_modifier = 0;

    for (int i = 0; i <= players.size(); i++) {
        float player_dps =
            players[i]->GetItemLevel() * players[i]->GetPerformanceRating() * dps_power;

        dps_power += player_dps;
    }
}