#include "fight.h"

Fight::Fight(const std::vector<Player*> players, Boss& boss) : players(players), boss(boss) {}

Fight::~Fight() {}

// ############### HELPER FUNCTIONS ############### //
std::vector<Player*> Fight::Fight::get_targetted_player(Role role_selection, int num_of_targets){
    std::vector<Player*> targetted_list_options;
    std::vector<Player*> final_targets;
    Player* random_player;
    for(int i = 0; i < num_of_targets; i++){
        for(auto player : m_alive_players) {
            Spec player_spec = player->GetSpec();
            if(player_spec.getRole() == role_selection){
                targetted_list_options.push_back(player);
            }
        }
        std::random_device rd;
        std::mt19937 gen(rd());
        if(!targetted_list_options.empty()){
            std::uniform_int_distribution<> dist(0, targetted_list_options.size() - 1);
            random_player = targetted_list_options[dist(gen)];
            final_targets.push_back(random_player);
        }
        // if there are no tanks choose a random player to get meleed
        else {
            std::uniform_int_distribution<> dist(0, m_alive_players.size() - 1);
            random_player = m_alive_players[dist(gen)];
            final_targets.push_back(random_player);
        }
    }
    return final_targets;
}


/* ##### DPS FUNCTIONS #####*/

float ilvl_factor(float player_ilvl, float boss_tuning_ilvl) {
    float tuning_factor = 2.8f;

    float delta = (player_ilvl - boss_tuning_ilvl) / boss_tuning_ilvl;
    float factor = 1.0f + (delta * tuning_factor);

    return std::clamp(factor, 0.4f, 1.25f);
}

float Fight::crit_multiplier(const Player& p) {
    Spec player_spec = p.GetSpec();
    auto stat_weight_struct = player_spec.getStatWeights();

    int player_crit = p.GetTotalCritStrike();
    float crit_multiplier = 1 + (player_crit / 100) * stat_weight_struct.critStrike;

    return crit_multiplier;
}

float Fight::haste_multiplier(const Player& p) {
    Spec player_spec = p.GetSpec();
    auto stat_weight_struct = player_spec.getStatWeights();

    int haste = p.GetTotalHaste();
    float haste_multiplier = 1 + (haste / 100) * stat_weight_struct.haste;

    return haste_multiplier;
}

float Fight::resist_profile(const Player& p) {
    Spec player_spec = p.GetSpec();
    DamageType player_damage = player_spec.getDamageType();

    float damage_resist = boss.getBossResistType(player_damage);

    return damage_resist;
}

float Fight::get_fight_affinity(const Player& p, Phase phase) {
    Spec player_spec = p.GetSpec();

    float fight_affinity_float = 1.0f;
    for (int i = 0; i < (int)phase.fightTypes.size(); i++) {
        auto currentFightType = phase.fightTypes[i];
        fight_affinity_float = player_spec.getFightAffinity(currentFightType);
    }

    return fight_affinity_float;
}

void Fight::single_target_heal_player(Player* p){
    if(p->getCurrentHealth() <= 0) {
        return;
    }

    static std::random_device rd;
    static std::mt19937 gen(rd());

    std::vector<Player*> healers;
    for(auto player : m_alive_players){
        Spec p_spec = player->GetSpec();
        if(p_spec.getRole() == Role::Healer){
            healers.push_back(player);
        }
    }

    for(auto p_healer : healers){
        HealerState* p_healerState = p_healer->getHealerState();
        std::vector<Spell> available_spells;
        for (const auto& spell : p_healerState->healing_spells) {
            if (!spell.isAoe) available_spells.push_back(spell);
        }
        if (!available_spells.empty()) {
            std::uniform_int_distribution<> dist(0, available_spells.size() - 1);
            Spell& chosen = available_spells[dist(gen)];
            p->heal(chosen.heal_value);
            p_healerState->current_mana -= chosen.mana_cost;
        }
    }
}

void Fight::aoe_heal_player(std::vector<Player*> player_list){
    std::vector<Player*> healers;
    for(auto player : m_alive_players){
        Spec p_spec = player->GetSpec();
        if(p_spec.getRole() == Role::Healer){
            healers.push_back(player);
        }
    }
    for(auto p_healer : healers){
        Spell aoe_spell;
        HealerState* p_healerState = p_healer->getHealerState();
        for(auto spell : p_healerState->healing_spells){
            if(spell.isAoe == true){
                aoe_spell = spell;
            }
        }
        for(auto p : player_list){
            p->heal(aoe_spell.heal_value);
        }
        p_healerState->current_mana -= aoe_spell.mana_cost;
    }
}


static float getVarianceFloor(float player_performance) {
    if (player_performance <= 50) {
        return .50f;
    } else if (player_performance <= 60) {
        return .55f;
    } else if (player_performance <= 70) {
        return .63f;
    } else if (player_performance <= 80) {
        return .70f;
    } else {
        return .85f;
    }
}

float Fight::calculate_player_dps(std::vector<Player*> active_player_list, float boss_ilvl,
                                  Phase bossPhase) {
    float total_dps = 0.0f;
    std::random_device rd;
    std::mt19937 gen(rd());

    m_current_debug_data.clear();

    for (int i = 0; i < (int)active_player_list.size(); i++) {
        Spec player_spec = active_player_list[i]->GetSpec();
        float ilvl_calculation = ilvl_factor(active_player_list[i]->GetItemLevel(), boss_ilvl);
        float base_dps = ilvl_calculation * active_player_list[i]->GetPerformanceRating() *
                         player_spec.getDPSWeight();

        float crit_multiplier_final  = crit_multiplier(*active_player_list[i]);
        float haste_multiplier_final = haste_multiplier(*active_player_list[i]);
        float boss_resist            = resist_profile(*active_player_list[i]);
        float fight_affinity         = get_fight_affinity(*active_player_list[i], boss.getCurrentPhase());

        std::uniform_real_distribution<float> dis(
            getVarianceFloor(active_player_list[i]->GetPerformanceRating()), MAX_VARIANCE);
        float variance = dis(gen);

        float player_dps = (base_dps * crit_multiplier_final * haste_multiplier_final *
                            (1.0f - boss_resist) * fight_affinity) * variance;

        FightDebugData fightDebugInfo;
        fightDebugInfo.player_performance_rating = active_player_list[i]->GetPerformanceRating();
        fightDebugInfo.player_dps        = player_dps;
        fightDebugInfo.ilvl_calculation  = ilvl_calculation;
        fightDebugInfo.crit_multiplier   = crit_multiplier_final;
        fightDebugInfo.haste_multiplier  = haste_multiplier_final;
        fightDebugInfo.boss_resist       = boss_resist;
        fightDebugInfo.fight_affinity    = fight_affinity;
        fightDebugInfo.variance          = variance;

        m_current_debug_data.push_back({ active_player_list[i]->GetName(), fightDebugInfo });
        total_dps += player_dps;
    }

    return total_dps;
}

//Maybe want to make this into a Map and decouple Spell from targets.
std::vector<Spell> Fight::damageStack(Boss &boss, float phase_duration) {
    Phase boss_phase = boss.getCurrentPhase();
    BossSpellDictionary spellDictionary = boss_phase.attackDictionary;

    float melee_delay     = spellDictionary.melee_delay;
    float cast_delay      = spellDictionary.cast_delay;
    float mechancic_delay = spellDictionary.mechanic_delay;

    int num_of_melee_casts    = std::floor(phase_duration / melee_delay);
    int num_of_spell_casts    = std::floor(phase_duration / cast_delay);
    int num_of_mechanic_casts = std::floor(phase_duration / mechancic_delay);

    std::vector<Spell> boss_damage_stack;

    for(int i = 0; i < num_of_melee_casts; i++) {
        Spell melee_spell;
        melee_spell.spell_name         = boss.GetName() + "_MELEE";
        melee_spell.spell_id           = -1;
        melee_spell.damage_value       = boss.getMeleeAttackValue();
        melee_spell.isAoe              = false;
        melee_spell.number_of_targets  = 1;
        melee_spell.target_list        = Fight::get_targetted_player(Role::Tank, melee_spell.number_of_targets);
        boss_damage_stack.push_back(melee_spell);
    }
    for(int j = 0; j < num_of_spell_casts; j++) {
        Spell ranged_spell;
        ranged_spell.spell_name        = boss.GetName() + "_SPELL";
        ranged_spell.spell_id          = -1;
        ranged_spell.damage_value      = boss.getSpellAttackValue();
        ranged_spell.isAoe             = false;
        ranged_spell.number_of_targets = 3;
        ranged_spell.target_list       = Fight::get_targetted_player(Role::DPS, ranged_spell.number_of_targets);
        boss_damage_stack.push_back(ranged_spell);
    }
    for (int k = 0; k < num_of_mechanic_casts; k++) {
        Spell mechanic_spell = boss_phase.mechanic_spell;
        mechanic_spell.target_list = Fight::get_targetted_player(
            mechanic_spell.isAoe ? Role::DPS : Role::Tank,
            mechanic_spell.number_of_targets
        );
        boss_damage_stack.push_back(mechanic_spell);
    }

    return boss_damage_stack;
}


std::vector<Player*> Fight::check_deaths() {
    std::vector<Player*> death_list;

    for (auto p : players) {
        if (p->getCurrentHealth() <= 0) {
            death_list.push_back(p);
        }
    }

    return death_list;
}

int Fight::resolveDamage(std::stack<Spell>& stack, int total_spells,
                         float current_boss_hp, float boss_hp_to_burn) {
    if (total_spells == 0) return 0;

    int spells_done    = 0;
    float hp_per_spell = boss_hp_to_burn / (float)total_spells;

    while (!stack.empty()) {
        Spell spell = stack.top();
        stack.pop();
        spells_done++;

        // Apply damage, skipping dead targets
        for (auto* p : spell.target_list) {
            if (p->getCurrentHealth() <= 0) continue;
            p->takeDamage(spell.damage_value);
        }

        // Heal after each attack
        if (spell.isAoe) {
            aoe_heal_player(spell.target_list);
        } else if (!spell.target_list.empty()) {
            single_target_heal_player(spell.target_list[0]);
        }

        // Build snapshot
        FightStep step;
        step.spell_name      = spell.spell_name;
        step.damage_value    = spell.damage_value;
        step.is_aoe          = spell.isAoe;
        step.spells_resolved = spells_done;
        step.total_spells    = total_spells;

        float burned_so_far    = hp_per_spell * (float)spells_done;
        step.boss_hp_remaining = current_boss_hp - burned_so_far;
        step.boss_hp_pct       = step.boss_hp_remaining / boss.getMaxHP();

        step.damage_data.damage_table[spell.spell_name] = spell.damage_value;
        step.player_debug = m_current_debug_data;

        float total_dps_step = 0.0f;
        for (const auto& [name, dbg] : m_current_debug_data)
            total_dps_step += dbg.player_dps;
        step.fight_state.current_dps = total_dps_step;
        step.fight_state.current_hps = 0.0f;

        // Record deaths at this step
        for (auto* p : m_alive_players) {
            if (p->getCurrentHealth() <= 0) {
                step.deaths_this_step.push_back(p->GetName());
            }
        }

        m_sim_history.push_back(step);

        if (!step.deaths_this_step.empty()) break;
    }

    return spells_done;
}

PhaseResult Fight::attemptPhase() {
    m_alive_players = players;
    m_sim_history.clear();

    Phase currentPhase = boss.getCurrentPhase();
    float boss_ilvl    = boss.GetBossilvl();
    float max_hp       = boss.getMaxHP();

    float phase_start_hp    = max_hp * currentPhase.hp_start_pct;
    float phase_end_hp      = max_hp * currentPhase.hp_end_pct;
    float remaining_phase_hp = phase_start_hp - phase_end_hp;
    float current_boss_hp    = phase_start_hp;

    float total_dps      = 0.0f;
    float total_duration = 0.0f;

    while (remaining_phase_hp > 0.0f && !m_alive_players.empty()) {
        total_dps = calculate_player_dps(m_alive_players, boss_ilvl, currentPhase);

        float time_for_segment = remaining_phase_hp / total_dps;

        auto spells_vec  = damageStack(boss, time_for_segment);
        int total_spells = (int)spells_vec.size();

        if (total_spells == 0) break;

        // Fill stack so first-built spell is processed first
        std::stack<Spell> boss_stack;
        for (auto it = spells_vec.rbegin(); it != spells_vec.rend(); ++it)
            boss_stack.push(*it);

        int done = resolveDamage(boss_stack, total_spells, current_boss_hp, remaining_phase_hp);

        float fraction     = (float)done / (float)total_spells;
        float hp_burned    = remaining_phase_hp * fraction;
        float time_passed  = time_for_segment * fraction;

        current_boss_hp   -= hp_burned;
        remaining_phase_hp -= hp_burned;
        total_duration    += time_passed;

        bool death_occurred = (done < total_spells);
        if (death_occurred) {
            m_alive_players.erase(
                std::remove_if(m_alive_players.begin(), m_alive_players.end(),
                    [](Player* p){ return p->getCurrentHealth() <= 0; }),
                m_alive_players.end());
            if (m_alive_players.empty()) break;
        } else {
            break;
        }
    }

    std::cout << "PHASE DURATION: " << total_duration << std::endl;

    PhaseResult result;
    result.actual_duration = total_duration;
    result.boss_hp_at_end  = current_boss_hp;
    result.total_dps       = total_dps;
    result.deaths          = (int)(players.size() - m_alive_players.size());
    result.survivied       = (int)m_alive_players.size();
    result.completed       = (remaining_phase_hp <= 0.0f);
    return result;
}

EncounterResult Fight::attemptFight() {
    int phase_count = boss.GetPhaseCount();

    EncounterResult fight_encounter;

    for (int i = 0; i < phase_count; i++) {
        PhaseResult result = attemptPhase();
        if (result.completed) {
            fight_encounter.total_dps      += result.total_dps;
            fight_encounter.actual_duration += result.actual_duration;
            fight_encounter.boss_hp_at_end  = result.boss_hp_at_end;
        }
        boss.AdvancePhase();
    }

    fight_encounter.survived = (int)m_alive_players.size();

    return fight_encounter;
}
