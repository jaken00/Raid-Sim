#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <random>
#include <queue>

#include "../Raid/Boss.h"
#include "../characters/player.h"

class Fight {
private:
    std::vector<Player*> players;
    std::vector<Player*> m_alive_players;
    Boss& boss;

    float MAX_VARIANCE = 1.30f;

    // ############### HELPER FUNCTIONS ############### //
    std::vector<Player*> get_targetted_player(Role role_selection, int num_of_targets);

    // ############### MATH HELPERS (reused per-spell) ############### //
    float resist_profile(const Player& p);
    float get_fight_affinity(const Player& p, Phase phase);

    // ############### V2 SPELL RESOLUTION ############### //
    float get_cast_interval(float base_cast_time, const Player& p);
    float calculate_spell_damage(Player* p, PlayerSpell& spell, bool did_crit, ActiveBuff& buffs);
    bool roll_crit(Player* p, ActiveBuff& buffs);

    // ############### DEATH TRACKING ############### //
    std::vector<Player*> check_deaths();

public:
    Fight(const std::vector<Player*> players, Boss& boss);
    ~Fight();

    FightLog simulate_encounter();
};

// Pure math — exposed for testing
float ilvl_factor(float player_ilvl, float boss_tuning_ilvl);
