#include "fight.h"

Fight::Fight(const std::vector<Player*> players, Boss& boss) : players(players), boss(boss) {}

Fight::~Fight() {}

// ############### HELPER FUNCTIONS ############### //

std::vector<Player*> Fight::get_targetted_player(Role role_selection, int num_of_targets) {
    std::vector<Player*> targetted_list_options;
    std::vector<Player*> final_targets;
    Player* random_player;
    for (int i = 0; i < num_of_targets; i++) {
        for (auto player : m_alive_players) {
            if (player->GetSpec().getRole() == role_selection) {
                targetted_list_options.push_back(player);
            }
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        if (!targetted_list_options.empty()) {
            std::uniform_int_distribution<> dist(0, targetted_list_options.size() - 1);
            random_player = targetted_list_options[dist(gen)];
            final_targets.push_back(random_player);
        } else {
            std::uniform_int_distribution<> dist(0, m_alive_players.size() - 1);
            random_player = m_alive_players[dist(gen)];
            final_targets.push_back(random_player);
        }
    }
    return final_targets;
}

// ############### MATH HELPERS ############### //

float ilvl_factor(float player_ilvl, float boss_tuning_ilvl) {
    float delta  = (player_ilvl - boss_tuning_ilvl) / boss_tuning_ilvl;
    float factor = 1.0f + (delta * 2.8f);
    return std::clamp(factor, 0.4f, 1.25f);
}

float Fight::resist_profile(const Player& p) {
    return boss.getBossResistType(p.GetSpec().getDamageType());
}

float Fight::get_fight_affinity(const Player& p, Phase phase) {
    float affinity = 1.0f;
    for (auto fightType : phase.fightTypes) {
        affinity = p.GetSpec().getFightAffinity(fightType);
    }
    return affinity;
}

// ############### V2 SPELL RESOLUTION ############### //

float Fight::get_cast_interval(float base_cast_time, const Player& p) {
    float haste_pct = (p.GetTotalHaste() / 100.0f) * p.GetSpec().getStatWeights().haste;
    return base_cast_time / (1.0f + haste_pct);
}

static float getVarianceFloor(float player_performance) {
    if (player_performance <= 50)      return 0.50f;
    else if (player_performance <= 60) return 0.55f;
    else if (player_performance <= 70) return 0.63f;
    else if (player_performance <= 80) return 0.70f;
    else                               return 0.85f;
}

float Fight::calculate_spell_damage(Player* p, PlayerSpell& spell, bool did_crit, ActiveBuff& buffs) {
    static std::mt19937 gen(std::random_device{}());

    float base = spell.base_damage
        * ilvl_factor(p->GetItemLevel(), boss.GetBossilvl())
        * p->GetPerformanceRating()
        * resist_profile(*p)
        * get_fight_affinity(*p, boss.getCurrentPhase());

    base *= buffs.damage_mult;

    float crit_mult = did_crit ? 2.0f : 1.0f;

    std::uniform_real_distribution<float> dis(getVarianceFloor(p->GetPerformanceRating()), MAX_VARIANCE);
    float variance = dis(gen);

    return base * crit_mult * variance;
}

bool Fight::roll_crit(Player* p, ActiveBuff& buffs) {
    float crit_chance = (p->GetTotalCritStrike() / 100.0f) * p->GetSpec().getStatWeights().critStrike;
    crit_chance += buffs.crit_bonus;

    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<float> dis(0.0f, 1.0f);
    return dis(gen) < std::clamp(crit_chance, 0.0f, 0.95f);
}

// ############### DEATH TRACKING ############### //

std::vector<Player*> Fight::check_deaths() {
    std::vector<Player*> death_list;
    for (auto p : players) {
        if (p->getCurrentHealth() <= 0) {
            death_list.push_back(p);
        }
    }
    return death_list;
}
