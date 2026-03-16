#pragma once

#include <string>
#include <vector>

#include "Raid.h"



struct BossMechanic {
    std::string name;
    float damageValue;
    bool needsInterrupt;

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
    std::vector<BossMechanic> mechanics;

public:
    Boss(const std::string& name, int id, Raid* raid, float currentHP, float maxHP, int phaseCount, int currentPhase,
         const std::vector<BossMechanic>& mechanics);
    ~Boss();
};