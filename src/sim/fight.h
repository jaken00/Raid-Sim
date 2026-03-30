#pragma once

#include <string>
#include <vector>
#include <algorithm>
#include <iostream>


#include "../Raid/Boss.h"
#include "../characters/player.h"


struct FightState {
    float current_dps;
    float current_hps;
    DefensiveState current_defensive_state; 
};

class Fight {
private:
    std::vector<Player*> players;
    Boss& boss;

    float getPlayerDPS();
    std::vector<HealerState*> getHealerState();
    float getPlayerDefense();
    float getPlayerUtility(); // figure out what this means and what this really does? I think this is imporatnt -> Maybe buffer or utility type? HPS buffer / Def / DPS
    
    float ilvl_factor(float player_ilvl, float boss_tuning_ilvl);
    float crit_multiplier(const Player& p);
    float haste_multiplier(const Player& p);
    float resist_profile(const Player& p);
    float get_fight_affinity(const Player& p, Phase phase);

    float effective_mana_cost_per_second(HealerState& h);
    float effective_hps(const HealerState &h, float phase_duration);

    DefensiveState calculate_defensive(); 
    bool combatResurectionNeeded(DefensiveState& defensiveState);

    FightState updateFightState(FightState& fightState);
    

public:
    Fight(const std::vector<Player*> players, Boss& boss);
    ~Fight();
    
    PhaseResult attemptPhase();

};