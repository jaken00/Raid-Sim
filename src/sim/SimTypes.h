#pragma once
#include <string>
#include <vector>
#include <map>

class Player;

struct Spell {
    bool isAoe;
    int number_of_targets;
    float damage_value;
    std::string spell_name;
    float mana_cost;
    std::vector<Player*> target_list;
    float heal_value;
    float shield_amount;
    bool provides_buff;
    int spell_id;
    float cooldown;       // 0 = no cooldown, >0 = seconds
    bool is_hot;          // heal over time
    std::string spec_name; // owning spec (e.g. "Mender")
};

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
    std::vector<Spell> healing_spells;
    float max_mana;
    float current_mana;
    float mana_regen_per_resolve_phase; //How much mana we recover after each damage phase call   
	std::vector<Spell> healing_priority; 
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
