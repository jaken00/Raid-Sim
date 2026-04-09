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

static float getVarianceFloor(float player_performance){
    if(player_performance <= 50){
        return .50f;
    } else if (player_performance <= 60)
    {
        return .55f;
    } else if (player_performance <= 70)
    {
        return .63f;
    } else if (player_performance <= 80)
    {
        return .70f;
    } else {
        return .85f;
    }
}
/*
ilvl_factor	clamped at 1.25	1.25
performance	100 / 100	1.0
dps_weight	Striker	1.05
crit_multiplier	1 + (crit/100) * 1.0	depends on item stats
haste_multiplier	1 + (haste/100) * 0.9	depends on item stats
boss_resist	1 - 0	1.0
fight_affinity	Striker execute_modifier	1.25
variance	MAX_VARIANCE	1.30
Ignoring crit/haste (currently 0 since all items are placeholders):


1.25 × 1.0 × 1.05 × 1.0 × 1.0 × 1.0 × 1.25 × 1.30 = ~2.14× MAX_DPS
So if you want a godlike performance-100 Striker to hit say 20,000 DPS at 
the absolute ceiling:


MAX_DPS = 20000 / 2.14 ≈ 9,346
A round MAX_DPS = 10,000 would put that ceiling at ~21,400.

One thing to flag: crit_multiplier and haste_multiplier have no cap, 
so once you add real item stats they could inflate this unboundedly. 
You probably want to clamp those too — 
something like std::clamp(crit_multiplier, 1.0f, 2.0f) once you 
define your stat ranges


*/



/* ##### ENCOUNTER/RAID FUNCTIONS #####*/

void Fight::takeDamage(float boss_damage, Player& p){ //might be over engineering and can just call it in the loop? (Maybe not sicne we have to recalculate hp values?)
    p.takeDamage(boss_damage);
}

std::vector<Player*> Fight::check_deaths(){
    std::vector<Player*> death_list; 
    
    for(auto p : players){
        if(p->getCurrentHealth() <= 0){
            death_list.push_back(p);
        }
    }

    return death_list;
}

PhaseResult Fight::attemptPhase(){
    std::random_device rd; 
    std::mt19937 gen(rd()); 
    PhaseResult phase_end_result;
    float total_dps = 0.0f;
    float base_dps = 0.0f;
    float total_hps = 0.0f;

    Phase currentPhase = boss.getCurrentPhase();
    // WE ARENT USING ACTUACL PHASE NUMBERS
    //currentPhase.hp_start_pct = 1.0f;
    //currentPhase.hp_end_pct = 0.0f;

    float boss_phase_hp_pool = (currentPhase.hp_start_pct - currentPhase.hp_end_pct) * boss.getMaxHP();
    
    for(int i = 0 ; i < players.size(); i++){
        Spec player_spec = players[i]->GetSpec(); 
        float ilvl_calculation = ilvl_factor(players[i]->GetItemLevel(), boss.GetBossilvl());
        base_dps = ilvl_calculation * players[i]->GetPerformanceRating() * player_spec.getDPSWeight();
        
        float crit_multiplier_final = crit_multiplier(*players[i]);
        float haste_multiplier_final = haste_multiplier(*players[i]);
        float boss_resist = resist_profile(*players[i]);
        float fight_affinity = get_fight_affinity(*players[i], boss.getCurrentPhase());
        std::uniform_real_distribution<float> dis(getVarianceFloor(players[i]->GetPerformanceRating()), MAX_VARIANCE);

        float variance = dis(gen);
        float real_boss_resist = 1.0f - boss_resist;

        float player_dps = (base_dps * crit_multiplier_final * haste_multiplier_final * (1.0f - boss_resist) * fight_affinity) * variance;
        /*
        std::cout << "ILVL CALCULATION: " << ilvl_calculation << std::endl;
        std::cout << "GET PLAYER PERFORMACE: " << players[i]->GetPerformanceRating() << std::endl;
        std::cout << "CRIT MULTIPLIER: " << crit_multiplier_final << std::endl;
        std::cout << "HASTE MULTIPLIER: " << haste_multiplier_final << std::endl;
        std::cout << "BOSS RESIST: " << real_boss_resist << std::endl;
        std::cout << "FIGHT AFFINITY : " << fight_affinity << std::endl;
        std::cout << "VARIANCE : " << variance << std::endl;
        std::cout << "PLAYER DPS: " << player_dps << std::endl;
        std::cout << "###########################################################" << std::endl;
        */
        
        total_dps += player_dps;

    }
    float phase_duration = boss_phase_hp_pool / total_dps;
    BossMechanic current_mechanic = currentPhase.mechanicAssociated;
    float boss_damage = current_mechanic.damageValue * players.size() * phase_duration;
    float damage_per_player = boss_damage / players.size();

    for(auto p : players){
        takeDamage(damage_per_player, *p);
    }
    std::vector<Player*> death_list = check_deaths();
    if(!death_list.empty()){ // IS NOT EMPTY
        std::cout << "RECALCULATE DPS NUMBERS DUE TO DEATH" << std::endl;   //How do we calcualcate this? pick a random time during the pull? 
        for(auto player : death_list){
            auto location = std::find(players.begin(), players.end(), player);
            players.erase(location); 
            attemptPhase();  
        }
        // HERE WILL RECALCUALTE DPS NUMBERS
    }

    phase_end_result.actual_duration = phase_duration;
    phase_end_result.boss_hp_at_end = 0.0;
    phase_end_result.total_dps = total_dps;
    phase_end_result.deaths = 0;
    phase_end_result.survivied = players.size();
    phase_end_result.completed = true; // FIX THIS HARDCODED COMPLETION


    //std::cout << "TOTAL DPS: "<< total_dps << std::endl;
    
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
