#pragma once

#include <string>
#include <vector>

#include "Raid.h"



struct BossMechanic {
    std::string name;
    float damageValue;
    bool needsInterrupt;
    

};

struct Phase {
    BossMechanic mechanicAssociated;
    int phaseNumber;
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

public:
    Boss(const std::string& name, int id, Raid* raid, float currentHP, float maxHP, int phaseCount, int currentPhase,
         const std::vector<Phase*> bossPhases);
    ~Boss();
};