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

    return std::clamp(factor, 0.4f, 1.25f);  // Clamp the fucntion min .4 penalty and max 1.25 bonus
                                             // (can change later for farming old raids??)
}

float Fight::crit_multiplier(const Player& p) {
    Spec player_spec = p.GetSpec();
    auto stat_weight_struct = player_spec.getStatWeights();

    // haste mult = 1 + (haste rating / 100) * spec stat weight
    int player_crit = p.GetTotalCritStrike();

    float crit_multiplier = 1 + (player_crit / 100) * stat_weight_struct.critStrike;

    return crit_multiplier;
}

float Fight::haste_multiplier(const Player& p) {
    Spec player_spec = p.GetSpec();
    auto stat_weight_struct = player_spec.getStatWeights();

    // haste mult = 1 + (haste rating / 100) * spec stat weight
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
    for (int i = 0; i < phase.fightTypes.size(); i++) {
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
    for(auto player :m_alive_players ){
        Spec p_spec = player->GetSpec();
        if(p_spec.getRole() == Role::Healer){
            healers.push_back(player);
        }
    }

    for(auto p_healer : healers){
        std::vector<Spell> available_spells;
        HealerState* p_healerState = p_healer->getHealerState();
        available_spells = p_healerState->healing_spells;
        int i = 0;
        for(auto spell : available_spells) {

            if(spell.isAoe == false){
                available_spells.erase(
                    std::remove_if(
                        available_spells.begin(),
                        available_spells.end(),
                        [](const Spell& spell) {
                            return !spell.isAoe;
                        }
                    ),
                    available_spells.end()
                );
            }
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
    for(auto player :m_alive_players ){
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



/* ##### HEALING FUNCTIONS #####*/
//KNOWN BUG: IF TARGET LIST HAS DEAD PLAYER WILL TARGET DEAD PLAYER
void Fight::resolveDamage(std::vector<Spell> boss_damage_stack){
    std::vector<Player*> healers;
    std::vector<Player*> unhealthy_players;

    for(auto boss_ability : boss_damage_stack) {
        for(auto p : boss_ability.target_list) {
            p->takeDamage(boss_ability.damage_value);
            boss_damage_stack.pop_back();
            if(p->getCurrentHealth() <= 0){
                m_alive_players.erase(std::remove(m_alive_players.begin(), m_alive_players.end(), p), m_alive_players.end());
            }
            if(boss_ability.isAoe == true){
                aoe_heal_player(boss_ability.target_list);
            }
            else {
                single_target_heal_player(boss_ability.target_list[0]); //this is so janky
            }
        }
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

    for (int i = 0; i < active_player_list.size(); i++) {
        Spec player_spec = active_player_list[i]->GetSpec();
        float ilvl_calculation = ilvl_factor(active_player_list[i]->GetItemLevel(), boss_ilvl);
        float base_dps = ilvl_calculation * active_player_list[i]->GetPerformanceRating() *
                         player_spec.getDPSWeight();

        float crit_multiplier_final = crit_multiplier(*active_player_list[i]);
        float haste_multiplier_final = haste_multiplier(*active_player_list[i]);
        float boss_resist = resist_profile(*active_player_list[i]);
        float fight_affinity = get_fight_affinity(*active_player_list[i], boss.getCurrentPhase());
        std::uniform_real_distribution<float> dis(
            getVarianceFloor(active_player_list[i]->GetPerformanceRating()), MAX_VARIANCE);

        float variance = dis(gen);
        float real_boss_resist = 1.0f - boss_resist;

        float player_dps = (base_dps * crit_multiplier_final * haste_multiplier_final *
                            (1.0f - boss_resist) * fight_affinity) *
                           variance;

        FightDebugData fightDebugInfo;

        fightDebugInfo.player_dps = player_dps;
        fightDebugInfo.ilvl_calculation = ilvl_calculation;
        fightDebugInfo.crit_multiplier = crit_multiplier_final;
        fightDebugInfo.haste_multiplier = haste_multiplier_final;
        fightDebugInfo.boss_resist = boss_resist;
        fightDebugInfo.fight_affinity = fight_affinity;
        fightDebugInfo.variance = variance;
        total_dps += player_dps;
    }

    return total_dps;
}

//Maybe want to make this into a Map and decouple Spell from targets. Right now Spells Owns the Target might want to remove that and do dynamic target checking in future (if someone dies later in phase)
//
std::vector<Spell> Fight::damageStack(Boss &boss, float phase_duration) {
    Phase boss_phase = boss.getCurrentPhase();
    BossSpellDictionary spellDictionary = boss_phase.attackDictionary;

    float melee_delay       = spellDictionary.melee_delay;
    float cast_delay        = spellDictionary.cast_delay;
    float mechancic_delay   = spellDictionary.mechanic_delay;

    int num_of_melee_casts      = std::floor(phase_duration / melee_delay);
    int num_of_spell_casts      = std::floor(phase_duration / cast_delay);
    int num_of_mechanic_casts   = std::floor(phase_duration / mechancic_delay);

    std::vector<Spell> boss_damage_stack;

    for(int i = 0; i < num_of_melee_casts; i++) {
        Spell melee_spell;

        melee_spell.spell_name = boss.GetName() + "_MELEE";
        melee_spell.spell_id = -1;
        melee_spell.damage_value = boss.getMeleeAttackValue();
        melee_spell.isAoe = false;
        melee_spell.number_of_targets = 1;
        melee_spell.target_list = Fight::get_targetted_player(Role::Tank, melee_spell.number_of_targets);
        boss_damage_stack.push_back(melee_spell);
    }
    for(int j = 0; j < num_of_spell_casts; j++) {
        Spell ranged_spell;

        ranged_spell.spell_name = boss.GetName() + "_SPELL";
        ranged_spell.spell_id = -1;
        ranged_spell.damage_value = boss.getSpellAttackValue();
        ranged_spell.isAoe = false;
        ranged_spell.number_of_targets = 3;
        ranged_spell.target_list = Fight::get_targetted_player(Role::DPS, ranged_spell.number_of_targets);
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

PhaseResult Fight::attemptPhase() {
    std::random_device rd;
    std::mt19937 gen(rd());
    PhaseResult phase_end_result;

    m_alive_players = players;

    Phase currentPhase = boss.getCurrentPhase();
    float boss_ilvl = boss.GetBossilvl();

    float boss_phase_hp_pool =
        (currentPhase.hp_start_pct - currentPhase.hp_end_pct) * boss.getMaxHP();

    float total_dps = calculate_player_dps(m_alive_players, boss_ilvl, currentPhase);

    float phase_duration = boss_phase_hp_pool / total_dps;
    Spell current_mechanic = currentPhase.mechanic_spell;
    std::vector<Spell> boss_damage_stack = damageStack(boss, phase_duration);
    resolveDamage(boss_damage_stack);


    std::vector<Player*> death_list = check_deaths();
    if (!death_list.empty()) {
        std::cout << "RECALCULATE DPS NUMBERS DUE TO DEATH"
                  << std::endl;  // How do we calcualcate this? pick a random time during the pull?
        for (auto player : death_list) {
            auto location = std::find(m_alive_players.begin(), m_alive_players.end(), player);
            m_alive_players.erase(location);
            calculate_player_dps(m_alive_players, boss_ilvl,
                                 currentPhase);  // Call dps calculation without Player alive
        }
    }

    phase_end_result.actual_duration = phase_duration;
    phase_end_result.boss_hp_at_end = 0.0;
    phase_end_result.total_dps = total_dps;
    phase_end_result.deaths = 0;
    phase_end_result.survivied = players.size();
    phase_end_result.completed = true;  // FIX THIS HARDCODED COMPLETION

    // std::cout << "TOTAL DPS: "<< total_dps << std::endl;

    std::cout << "PHASE DURATION: " << phase_duration << std::endl;

    return phase_end_result;
}

EncounterResult Fight::attemptFight() {
    int phase_count = boss.GetPhaseCount();

    EncounterResult fight_encounter;

    for (int i = 0; i < phase_count; i++) {
        PhaseResult result = attemptPhase();
        if (result.completed) {
            fight_encounter.total_dps += result.total_dps;
            fight_encounter.actual_duration += result.actual_duration;
            fight_encounter.boss_hp_at_end = result.boss_hp_at_end;
        }
        boss.AdvancePhase();
    }

    fight_encounter.survived = 10;

    return fight_encounter;
}
