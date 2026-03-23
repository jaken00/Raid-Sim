#include "fight.h"

Fight::Fight(const std::vector<Player*> players, Boss& boss) : players(players), boss(boss) {}

Fight::~Fight() {}

float Fight::ilvl_factor(int player_ilvl, int boss_tuning_ilvl){
    float tuning_factor = 2.8f;

    float delta = (player_ilvl - boss_tuning_ilvl) / boss_tuning_ilvl;
    float factor = 1.0f + (delta * tuning_factor); 

    return std::clamp(0.4f, factor, 1.25f); // Clamp the fucntion min .4 penalty and max 1.25 bonus (can change later for farming old raids??)
}


float Fight::crit_multiplier(const Player &p){
    float current_player_crit = p.GetItemLevel(); // -> Need to add a function to get curret player Stats (can be &)

}