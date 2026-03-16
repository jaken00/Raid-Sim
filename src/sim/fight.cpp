#include "fight.h"

Fight::Fight(const std::vector<Player*>& players, Boss* boss) : players(players), boss(boss) {}

Fight::~Fight() {}

float Fight::getPlayerDPS(){

    float total_dps = 0.0;

    for (int i = 0; i <= players.size(); i++) {
        Spec player_spec = players[i]->GetSpec();
        float player_dps =
            players[i]->GetItemLevel() * players[i]->GetPerformanceRating() * player_spec.getDPSWeight();

        total_dps += player_dps;
    }


    return total_dps;
}

float Fight::getPlayerHPS(){

    float total_hps = 0.0;

    for (int i = 0; i <= players.size(); i++) {
        Spec player_spec = players[i]->GetSpec();
        float player_hps =
            players[i]->GetItemLevel() * players[i]->GetPerformanceRating() * player_spec.getHPSweight();

        total_hps += player_hps;
    }


    return total_hps;
}
float Fight::getPlayerDefense(){

    float total_defense = 0.0;

    for (int i = 0; i <= players.size(); i++) {
        Spec player_spec = players[i]->GetSpec();
        float player_defense =
            players[i]->GetItemLevel() * players[i]->GetPerformanceRating() * player_spec.getDefenseWeight();

        total_defense += player_defense;
    }


    return total_defense;
}
float Fight::getPlayerUtility(){

    float total_utility = 0.0;

    for (int i = 0; i <= players.size(); i++) {
        Spec player_spec = players[i]->GetSpec();
        float player_utility =
            players[i]->GetItemLevel() * players[i]->GetPerformanceRating() * player_spec.getUtilityWeight();

        total_utility += player_utility;
    }


    return player_utility;
}



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