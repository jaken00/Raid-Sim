#include "fight.h"

Fight::Fight(const std::vector<Player*> players, Boss& boss) : players(players), boss(boss) {}

Fight::~Fight() {}

/* ##### DPS FUNCTIONS #####*/

float Fight::ilvl_factor(float player_ilvl, float boss_tuning_ilvl){
    float tuning_factor = 2.8f;

    float delta = (player_ilvl - boss_tuning_ilvl) / boss_tuning_ilvl;
    float factor = 1.0f + (delta * tuning_factor);

    return std::clamp(factor, 0.4f , 1.25f); // Clamp the fucntion min .4 penalty and max 1.25 bonus (can change later for farming old raids??)
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

/* ##### HEALING FUNCTIONS #####*/

float Fight::effective_mana_cost_per_second(HealerState& h){
    float proc_chance = h.player->GetTotalExpertise() / 500.0f; 
    float proc_chance_capped = std::min(proc_chance, 0.35f);
    float mana_saved_factor = 1.0f - (proc_chance_capped * 0.6); // proc saves 60% mana

    return h.mana_per_second_cost * mana_saved_factor;
}

float Fight::effective_hps(HealerState &h, float phase_duration){
    float actual_cost = effective_mana_cost_per_second(h);
    float mana_needed = h.mana_per_second_cost * phase_duration;

    if(mana_needed <= h.current_mana) {
        h.current_mana = h.current_mana - mana_needed;
        return h.hps_at_full_mana;
    }

    float full_time = h.current_mana / actual_cost;
    float oom_time = phase_duration - full_time;

    return (h.hps_at_full_mana * full_time + h.hps_at_oom * oom_time) / phase_duration;

}


/* ##### ENCOUNTER/RAID FUNCTIONS #####*/

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

        //ADD IN VARIANCE
    }
    float phase_duration = boss_phase_hp_pool / total_dps;
    BossMechanic current_mechanic = currentPhase.mechanicAssociated;
    float boss_damage = current_mechanic.damageValue * players.size() * phase_duration;

    phase_end_result.actual_duration = phase_duration;
    phase_end_result.boss_hp_at_end = 0.0;
    phase_end_result.total_dps = total_dps;
    phase_end_result.deaths = 0;
    phase_end_result.survivied = players.size();
    phase_end_result.completed = true; // FIX THIS HARDCODED COMPLETION


    std::cout << "TOTAL DPS: "<< total_dps << std::endl;
    
    std::cout << "PHASE DURATION: "<< phase_duration << std::endl;
    std::cout << "BOSS DAMAGE: "<< boss_damage << std::endl;

    return phase_end_result;


}

EncounterResult Fight::attemptFight(){
    int phase_count = boss.GetPhaseCount();

    EncounterResult fight_encounter;

    for(int i = 0; i < phase_count; i++){
        PhaseResult result = attemptPhase();
        if(result.completed){
            fight_encounter.total_dps += result.total_dps;
            fight_encounter.actual_duration += result.actual_duration;
            fight_encounter.boss_hp_at_end = result.boss_hp_at_end;
        }
        boss.AdvancePhase();
    }

    fight_encounter.survived = 10;

    return fight_encounter;

}