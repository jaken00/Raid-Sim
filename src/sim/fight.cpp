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

float Fight::calculate_spell_damage(Player* p, PlayerSpell& spell, bool did_crit,
                                    ActiveBuffs& buffs, float current_time) {
    static std::mt19937 gen(std::random_device{}());

    float base = spell.base_damage
        * ilvl_factor(p->GetItemLevel(), boss.GetBossilvl())
        * p->GetPerformanceRating()
        * resist_profile(*p)
        * get_fight_affinity(*p, boss.getCurrentPhase());

    base *= buffs.get_damage_modifier(p, current_time);

    float crit_mult = did_crit ? 2.0f : 1.0f;

    std::uniform_real_distribution<float> dis(getVarianceFloor(p->GetPerformanceRating()), MAX_VARIANCE);
    float variance = dis(gen);

    return base * crit_mult * variance;
}

bool Fight::roll_crit(Player* p, ActiveBuffs& buffs, float current_time) {
    float crit_chance = (p->GetTotalCritStrike() / 100.0f) * p->GetSpec().getStatWeights().critStrike;
    crit_chance += buffs.get_crit_bonus(p, current_time); // TODO: THis might be wrong and instead of rolling for Crit Bonus we might need crit chance modifier?

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

FightLog Fight::simulate_encounter() {
    /*
    struct FightLog {
    FightResult result;
    float end_time;
    std::vector<CombatEvent> events;
    std::vector<DeathRecord> deaths;
    KillingBlow killing_blow;
    std::map<std::string, float> player_total_damage;
    std::map<std::string, float> player_total_healing;
    
    struct PendingCast {
    float timestamp;
    int source_id; //player_index or -1 for boss
    int spell_index; //index of spell in player rotation queue

    bool operator>(const PendingCast& other) const { return timestamp > other.timestamp; }
};

    */
    FightLog log;

    std::priority_queue<PendingCast, std::vector<PendingCast>, std::greater<PendingCast>> prio_queue;

    m_alive_players = players;
    for (int i = 0; i < (int)players.size(); i++) {
        prio_queue.push({0.0f, i, 0});
    }
    prio_queue.push({0.0f, -1, 0});  // boss

    while (!prio_queue.empty()) {
        PendingCast next = prio_queue.top();
        prio_queue.pop();
        float t = next.timestamp;

        if (t > boss.GetEnrageTimer()) {
            log.result = FightResult::ENRAGE;
            log.end_time = t;
            break;
        }

        if (next.source_id != -1) {
            Player* p = m_alive_players[next.source_id];
            PlayerSpell& spell = p->GetRotationSpell(next.spell_index);
            spell.ready_at = t + spell.cooldown;
            /*
              Player* p = players[next.source_id];
              PlayerSpell& spell = p->GetRotationSpell(next.spell_index);

              bool crit = roll_crit(p, buffs, t);
              float dmg = calculate_spell_damage(p, spell, crit, buffs, t);
              boss.takeDamage(dmg);

              log.events.push_back({t, p->GetName(), spell.name, spell.base_damage, dmg, crit, EventType::DAMAGE, true, "Boss"});

              // requeue
              prio_queue.push({t + get_cast_interval(spell.base_cast_time, *p), next.source_id,
              p->GetNextSpellIndex(next.spell_index)});

              Pop → resolve → push back. That's the entire loop.
            
                  while (!prio_queue.empty() && !m_alive_players.empty() && boss.GetCurrentHP() > 0)

              With ||: loop continues as long as either condition is true — so it keeps running even after the boss dies as long as
              players are alive. Wrong.

              With &&: loop continues only while all conditions are true — stops the moment any one fails. Correct.

              That said, this actually isn't the primary exit mechanism anyway. The && conditions here are just safety nets. The
              real exits are the break statements inside the loop:

              // Kill
              if (boss.GetCurrentHP() <= 0) {
                  log.result = FightResult::KILL;
                  log.end_time = t;
                  break;
              }

              // Wipe
              if (m_alive_players.empty()) {
                  log.result = FightResult::WIPE;
                  log.end_time = t;
                  break;
              }

              // Enrage
              if (t > boss.GetEnrageTimer()) {
                  log.result = FightResult::ENRAGE;
                  log.end_time = t;
                  break;
              }


            */
        }

    }

}