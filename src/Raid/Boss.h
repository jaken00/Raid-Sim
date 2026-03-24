#pragma once

#include <string>
#include <vector>
#include <map>


#include "Raid.h"



struct BossResistanceProfile {
    float Physical;
    float Fire;
    float Storm;
    float Frost;
    float Shadow;
    float Radiant;
};

struct BossMechanic {
    std::string name;
    float damageValue;
    bool needsInterrupt;
    

};

struct Phase {
    BossMechanic mechanicAssociated;
    int phaseNumber;
    bool is_execute_phase;
};

class Boss {
private:
    std::string name;
    int id;
    int phaseCount;
    int currentPhase;
    Raid* raid;
    float currentHP;
    float maxHP;
    std::vector<Phase*> bossPhases;
    DamageType bossDamagetype;
    std::map<DamageType, float> resistMap;

public:
    Boss(const std::string& name, int id, Raid* raid, float currentHP, float maxHP, int phaseCount, int currentPhase,
         const std::vector<Phase*> bossPhases);
    ~Boss();



};

Boss::~Boss(){}

