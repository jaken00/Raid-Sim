#pragma once
#include <string>
#include <vector>
#include <map>
#include <utility>

class Player;

struct Spell {
    bool isAoe;
    int number_of_targets;
    float damage_value;
    std::string spell_name;
    std::string spec_name;
    float mana_cost;
    float heal_value;
    float shield_amount;
    bool provides_buff;
    bool is_hot;
    float cooldown;
    std::vector<Player*> target_list;
    int spell_id;
    bool operator<(const Spell& other) const { return spell_id < other.spell_id; }
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
    float max_mana;
    float current_mana;
    float mana_regen_per_resolve_phase; //How much mana we recover after each damage phase call
    std::vector<Spell> healing_spells;
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

struct FightState {
    float current_dps;
    float current_hps;
    DefensiveState current_defensive_state;
};

struct BossDamageStackData {
    std::map<std::string, float> damage_table;
};

struct FightDebugData {
    float player_performance_rating;
    float ilvl_calculation;
    float crit_multiplier;
    float haste_multiplier;
    float boss_resist;
    float fight_affinity;
    float variance;
    float player_dps;
};

struct HealResolutionData {
    Player* healer = nullptr;
    Player* target = nullptr;
    std::map<Spell, float> heal_table;
};

struct FightStep {
    std::string spell_name;
    float damage_value = 0.0f;
    bool is_aoe = false;

    float boss_hp_remaining = 0.0f;
    float boss_hp_pct       = 0.0f;
    int   spells_resolved   = 0;
    int   total_spells      = 0;

    std::vector<std::string> deaths_this_step;

    FightState          fight_state{};
    BossDamageStackData damage_data;
    HealResolutionData  heal_data;

    std::vector<std::pair<std::string, FightDebugData>> player_debug;
};
