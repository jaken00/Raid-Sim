#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <random>
#include <stack>

#include "../Raid/Boss.h"
#include "../characters/player.h"

class Fight {
private:
    std::vector<Player*> players;
    std::vector<Player*> m_alive_players;
    Boss& boss;

    float MAX_VARIANCE = 1.30f;

    std::vector<std::pair<std::string, FightDebugData>> m_current_debug_data;
    std::vector<FightStep> m_sim_history;

    float getPlayerDPS();
    std::vector<HealerState*> getHealerState();
    float getPlayerDefense();
    float getPlayerUtility();

    // ############### HELPER FUNCTIONS ############### //
    std::vector<Player*> get_targetted_player(Role role_selection, int num_of_targets);

    // ############### DPS CALCULATIONS ############### //
    float crit_multiplier(const Player& p);
    float haste_multiplier(const Player& p);
    float resist_profile(const Player& p);
    float get_fight_affinity(const Player& p, Phase phase);
    float calculate_player_dps(std::vector<Player*> active_player_list, float boss_ilvl, Phase bossPhase);

    // ############### HPS CALCULATIONS ############### //
    void single_target_heal_player(Player* p);
    void aoe_heal_player(std::vector<Player*> player_list);

    // ############### DEFENSE CALCULATIONS ############### //
    DefensiveState calculate_defensive();
    bool combatResurectionNeeded(DefensiveState& defensiveState);
    FightState updateFightState(FightState& fightState);

    // ############### DAMAGE TO PLAYERS ############### //
    void takeDamage(float boss_damage, Player& p);
    std::vector<Player*> check_deaths();

    // Returns spells resolved; stops early if a death occurs
    int resolveDamage(std::stack<Spell>& stack, int total_spells,
                      float current_boss_hp, float boss_hp_to_burn);

    // ############### DEBUG DATA ############### //
    void print_fight_data();

    // ############### BOSS CALCULATIONS ############### //
    std::vector<Spell> damageStack(Boss &boss, float phase_duration);

public:
    Fight(const std::vector<Player*> players, Boss& boss);
    ~Fight();

    PhaseResult attemptPhase();
    EncounterResult attemptFight();

    const std::vector<FightStep>& getSimHistory() const { return m_sim_history; }
};

// Pure math — exposed for testing
float ilvl_factor(float player_ilvl, float boss_tuning_ilvl);
