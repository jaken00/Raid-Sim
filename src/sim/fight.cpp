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

std::vector<HealerState*> Fight::getHealerState() {
    std::vector<HealerState*> healerStateHolder;

    HealerState* newHealerState = new HealerState{};
    healerStateHolder.push_back(newHealerState);

    return healerStateHolder; //TODO: Look at this as a unique pointer maybe to get rid of manual memory management. 
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


    return total_utility;
}


