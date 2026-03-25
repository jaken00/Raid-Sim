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
float Fight::resist_profile(const Player& p){
    Spec player_spec = p.GetSpec();
    DamageType player_damage = player_spec.getDamageType();

    float damage_resist = boss.getBossResistType(player_damage);

    return damage_resist;
}


float Fight::get_fight_affinity(const Player& p, Phase phase){

    Spec player_spec = p.GetSpec();

    float fight_affinity_float;

    for(int i = 0; i < phase.fightTypes.size(); i++){
          auto currentFightType = phase.fightTypes[i];
          fight_affinity_float = player_spec.getFightAffinity(currentFightType);
    }

    return fight_affinity_float;

}

PhaseResult Fight::attemptPhase(){
    //Do the fight part! 
}