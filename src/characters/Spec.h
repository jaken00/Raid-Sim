#include <string>

#include "../enum.h"
#include "../sim/SimTypes.h"
#include "PlayerStructTypes.h"

enum class AttackRange {
    Melee, 
    Healer,
    Caster,
    Ranged
};

struct SpecStatWeights {
    float critStrike;
    float haste;
    float expertise;
    float constitution; //Higher defense actively hurts dps values
    
};


class Spec {
private:
    std::string name;
    Resource resouceUsed; 
    AttackRange attackRange; 
    float dps_weight;
    DamageType specDamageType; 
    float hps_weight; 
    float defense_weight; 
    float utility_weight;
    std::string primary_stat;
    bool can_interrupt; 
    bool can_dispel; 
    bool provides_shield;
    bool provides_external_cd; 
    RaidBuff raid_buff; 
    float execute_bonus; 
    float aoe_modifier;
    FightAffinityProfile specFightProfile;
    SpecStatWeights specStatWeights;

    void setSpecWeights();
    
public:
    Spec(std::string name, Resource resouceUsed, AttackRange attackRange, float dps_weight,
         DamageType specDamageType, float hps_weight, float defense_weight, float utility_weight,
         std::string primary_stat, bool can_interrupt, bool can_dispel, bool provides_shield,
         bool provides_external_cd, RaidBuff raid_buff, float execute_bonus, float aoe_modifier,
         FightAffinityProfile specFightProfile);
    ~Spec();
    float getDPSWeight();
    float getHPSweight();
    float getDefenseWeight();
    float getUtilityWeight();
    SpecStatWeights getStatWeights();
    void initSpec();
};

inline Spec::Spec(std::string name, Resource resouceUsed, AttackRange attackRange, float dps_weight,
           DamageType specDamageType, float hps_weight, float defense_weight, float utility_weight,
           std::string primary_stat, bool can_interrupt, bool can_dispel, bool provides_shield,
           bool provides_external_cd, RaidBuff raid_buff, float execute_bonus,
           float aoe_modifier, FightAffinityProfile specFightProfile)
    : name(name),
      resouceUsed(resouceUsed),
      attackRange(attackRange),
      dps_weight(dps_weight),
      specDamageType(specDamageType),
      hps_weight(hps_weight),
      defense_weight(defense_weight),
      utility_weight(utility_weight),
      primary_stat(primary_stat),
      can_interrupt(can_interrupt),
      can_dispel(can_dispel),
      provides_shield(provides_shield),
      provides_external_cd(provides_external_cd),
      raid_buff(raid_buff),
      execute_bonus(execute_bonus),
      aoe_modifier(aoe_modifier),
      specFightProfile(specFightProfile)
{
    initSpec();
}

inline Spec::~Spec() {}

inline float Spec::getDPSWeight() {
    return dps_weight;
}

inline float Spec::getHPSweight() {
    return hps_weight;
}

inline float Spec::getDefenseWeight() {
    return defense_weight;
}

inline float Spec::getUtilityWeight() {
    return utility_weight;
}

inline SpecStatWeights Spec::getStatWeights() {
    return specStatWeights;
} 