#pragma once

#include <map>
#include <string>
#include <vector>

#include "../enum.h"
#include "../sim/SimTypes.h"
#include "Raid.h"

struct BossSpellDictionary {
    float melee_delay;
    float cast_delay;
    float mechanic_delay;
};

struct Phase {
    Spell mechanic_spell;
    int phaseNumber;
    bool is_execute_phase;
    std::vector<FightAffinityProfile> fightTypes;
    float hp_start_pct;
    float hp_end_pct;
    std::vector<Spell> phase_spells;
    BossSpellDictionary attackDictionary;
};

class Boss {
private:
    std::string name;
    int id;
    int phaseCount;
    int currentPhaseNumber;
    float ilvl_threshhold;
    Raid* raid;
    float currentHP;
    float maxHP;
    std::vector<Phase*> bossPhases;
    DamageType bossDamagetype;
    std::map<DamageType, float> resistMap;
    Phase current_phase;
    float melee_attack_value;
    float spell_attack_value;

public:
    Boss(const std::string& name, int id, Raid* raid, float currentHP, float maxHP, int phaseCount,
         int currentPhase, float ilvl_threshhold, const std::vector<Phase*> bossPhases,
         DamageType bossDamagetype, std::map<DamageType, float> resistMap,
         float melee_attack_value, float spell_attack_value);

    Boss()
        : id(0),
          phaseCount(0),
          currentPhaseNumber(0),
          ilvl_threshhold(0),
          raid(nullptr),
          currentHP(0),
          maxHP(0),
          bossDamagetype(DamageType::Physical),
          melee_attack_value(0.0f),
          spell_attack_value(0.0f) {}  // default construor

    ~Boss();

    float getBossResistType(DamageType damageType);
    float GetBossilvl();
    Phase getCurrentPhase();
    float GetCurrentHP();
    void AdvancePhase();

    std::string GetName() const;
    float getMaxHP() const;
    int GetPhaseCount();
    float getMeleeAttackValue() const { return melee_attack_value; }
    float getSpellAttackValue()  const { return spell_attack_value; }
};

inline float Boss::GetBossilvl() {
    return ilvl_threshhold;
}

inline Phase Boss::getCurrentPhase() {
    return current_phase;
}

inline float Boss::getMaxHP() const {
    return maxHP;
}

inline std::string Boss::GetName() const {
    return name;
}

inline float Boss::GetCurrentHP() {
    return currentHP;
}

inline int Boss::GetPhaseCount() {
    return phaseCount;
}
