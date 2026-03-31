#include "fight.h"

Fight::Fight(const std::vector<Player*> players, Boss& boss) : players(players), boss(boss) {}

Fight::~Fight() {}

float Fight::ilvl_factor(float player_ilvl, float boss_tuning_ilvl){
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

    float fight_affinity_float = 1.0f;

    for(int i = 0; i < phase.fightTypes.size(); i++){
          auto currentFightType = phase.fightTypes[i];
          fight_affinity_float = player_spec.getFightAffinity(currentFightType);
    }

    return fight_affinity_float;

}

PhaseResult Fight::attemptPhase(){
    
    PhaseResult phase_end_result;
    float total_dps = 0.0f;
    float base_dps = 0.0f;

    Phase currentPhase = boss.getCurrentPhase();
    currentPhase.hp_start_pct = 1.0f;
    currentPhase.hp_end_pct = 0.0f;

    float boss_phase_hp_pool = (currentPhase.hp_start_pct - currentPhase.hp_end_pct) * boss.getMaxHP();

    for(int i = 0 ; i < players.size(); i++){
        Spec player_spec = players[i]->GetSpec(); 
        float ilvl_calculation = ilvl_factor(players[i]->GetItemLevel(), boss.GetBossilvl());
        base_dps = ilvl_calculation * players[i]->GetPerformanceRating() * player_spec.getDPSWeight();
        float crit_multiplier_final = crit_multiplier(*players[i]);
        float haste_multiplier_final = haste_multiplier(*players[i]);
        float boss_resist = resist_profile(*players[i]);
        float fight_affinity = get_fight_affinity(*players[i], boss.getCurrentPhase());

        float player_dps = base_dps * crit_multiplier_final * haste_multiplier_final * (1.0f - boss_resist) * fight_affinity;

        total_dps += player_dps;
    }
    std::cout << "TOTAL DPS: "<< total_dps << std::endl;
    float phase_duration = boss_phase_hp_pool / total_dps;
    std::cout << "PHASE DURATION: "<< phase_duration << std::endl;

    phase_end_result.actual_duration = phase_duration;
    phase_end_result.boss_hp_at_end = 0.0;
    phase_end_result.total_dps = total_dps;
    phase_end_result.deaths = 0;
    phase_end_result.survivied = players.size();

    return phase_end_result;


}


EncounterResult Fight::attemptFight(){
    
}