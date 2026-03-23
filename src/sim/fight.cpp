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

    Spec player_spec = p.GetSpec(); 
    auto stat_weight_struct = player_spec.getStatWeights();
    
    // haste mult = 1 + (haste rating / 100) * spec stat weight
    int player_crit = p.GetTotalCritStrike();

    float crit_multiplier = 1 + (player_crit / 100) * stat_weight_struct.critStrike;

    return crit_multiplier;

}

float Fight::haste_multiplier(const Player& p){
        Spec player_spec = p.GetSpec(); 
    auto stat_weight_struct = player_spec.getStatWeights();
    
    // haste mult = 1 + (haste rating / 100) * spec stat weight
    int haste = p.GetTotalHaste();

    float haste_multiplier = 1 + (haste / 100) * stat_weight_struct.haste;

    return haste_multiplier;
}