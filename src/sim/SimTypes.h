#pragma once
#include <string>
#include <vector>
#include <map>

class Player;

enum struct FightAffinityProfile {
     single_target_modifier,  // pure patchwerk DPS
     aoe_modifier,            // 4+ targets
     cleave_modifier,         // 2–3 targets, council/adds
     movement_modifier,       // DoTs and instants love movement
     execute_modifier,        // bonus below 20% boss HP
     melee_hostile_penalty,   // bosses with ground effects, cleave, frontal
     MISSING_PROFILE
};

struct HealerState {
    std::string spec_name;
    float current_mana;
    float max_mana;
    float mana_per_second_cost;
    float hps_at_full_mana;
    float hps_at_oom;
};

struct DefensiveState {
    float shield_absorb;
    float damage_reduction_pct;
    float battle_ressurection_charges; 
}; 

struct PhaseResult {
    int survivied;
    float boss_hp_at_end;
    float actual_duration;
    float total_dps;
    int deaths;
    bool completed;
};

struct EncounterResult{
    int survived;
    float boss_hp_at_end;
    float actual_duration;
    float total_dps;
    float total_hps;
    std::map<Player*, float> player_dps;
    std::map<Player*, float> player_hps;  
     
};

struct RaidResult {
    float total_duration;
    std::vector<Player*> player_deaths;
    std::map<Player*, float> total_dps;    
    std::map<Player*, float> total_hps;
      
};

