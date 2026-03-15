#include <string>

#include "../enum.h"
#include "../sim/SimTypes.h"
#include "PlayerStructTypes.h"



class Spec {
private:
    /* data */
public:
    Spec(std::string name, Resource resouceUsed, AttackRange AttackRange, float dps_weight,
         DamageType specDamageType, float hps_weight, float defense_weight, float utility_weight,
         std::string primary_stat, bool can_interrupt, bool can_dispel, bool provides_shield,
         bool provides_external_cd, RaidBuff raid_buff, float execute_bonus, float aoe_modifier,
         FightAffinityProfile specFightProfile);
    ~Spec();
    float getDPSWeight();
    float getHPSweight();
    float getDefenseWeight();
    float getUtilityWeight();
};

Spec::Spec(std::string name, Resource resouceUsed, AttackRange AttackRange, float dps_weight,
           DamageType specDamageType, float hps_weight, float defense_weight, float utility_weight,
           std::string primary_stat, bool can_interrupt, bool can_dispel, bool provides_shield,
           bool provides_external_cd, RaidBuff raid_buff, float execute_bonus,
           float aoe_modifier, FightAffinityProfile specFightProfile) {}

Spec::~Spec() {}
