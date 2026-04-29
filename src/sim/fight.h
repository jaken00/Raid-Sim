#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>

#include "../Raid/Boss.h"
#include "../characters/player.h"


struct FightState {
    float current_dps;
    float current_hps;
    DefensiveState current_defensive_state;
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

class Fight {
private:
    std::vector<Player*> players;
    std::vector<Player*> m_alive_players;
    Boss& boss;

    float MAX_VARIANCE = 1.30f;
	std::vector<FightDebugData> debugData;
    float getPlayerDPS();
    std::vector<HealerState*> getHealerState();
    float getPlayerDefense();
    float getPlayerUtility(); // figure out what this means and what this really does? I think this is imporatnt -> Maybe buffer or utility type? HPS buffer / Def / DPS

    // ############### HELPER FUNCTIONS ############### //
    std::vector<Player*> get_targetted_player(Role role_selection, int num_of_targets);

    // ############### DPS CALCULATIONS ############### //
    float crit_multiplier(const Player& p);
    float haste_multiplier(const Player& p);
    float resist_profile(const Player& p);
    float get_fight_affinity(const Player& p, Phase phase);
	float calculate_player_dps(std::vector<Player*> active_player_list, float boss_ilvl, Phase bossPhase); //Main Calculation Function

    // ############### HPS CALCULATIONS ############### //
    void resolve_incoming_damage(); //This is needed to get the entire Spell as it carries all information

    // ############### DEFENSE CALCULATIONS ############### //
    DefensiveState calculate_defensive();
    bool combatResurectionNeeded(DefensiveState& defensiveState);
    FightState updateFightState(FightState& fightState);

    // ############### DAMAGE TO PLAYERS ############### //
    void takeDamage(float boss_damage, Player& p);
    std::vector<Player*> check_deaths();

	// ############### DEBUG DATA ############### //
	void print_fight_data();

	// ############### BOSS CALCULATIONS ############### //

	std::vector<Spell> damageStack(Boss *boss, float phase_duration);



public:
    Fight(const std::vector<Player*> players, Boss& boss);
    ~Fight();

    PhaseResult attemptPhase();
    EncounterResult attemptFight();
};

// Pure math — exposed for testing
float ilvl_factor(float player_ilvl, float boss_tuning_ilvl);
