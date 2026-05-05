#include <vector>
#include "../sim/SimTypes.h"
#include "Boss.h"


class Player;

struct PhaseBuilder {
    Spell b_mechanic_spell;
    int b_phaseNumber;
    bool b_is_execute_phase;
    std::vector<FightAffinityProfile> b_fightTypes;
    float b_hp_start_pct;
    float b_hp_end_pct;
    std::vector<Spell> b_phase_spells;
    BossSpellDictionary b_attackDictionary;
};

struct FightBuilder {
    std::vector<PhaseBuilder> b_phases;
    Boss* b_boss;
};

struct RaidBuilder {
    std::vector<FightBuilder> b_fights;
    std::vector<Player*> b_players;
};
