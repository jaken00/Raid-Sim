#pragma once

#include <string>
#include <vector>

#include "BossMechanic.h"
#include "Raid.h"

class Boss {
private:
    std::string name;
    int id;
    Raid* raid;
    float currentHP;
    float maxHP;
    std::vector<BossMechanic> mechanics;

public:
    Boss(const std::string& name, int id, Raid* raid, float currentHP, float maxHP,
         const std::vector<BossMechanic>& mechanics);
    ~Boss();
};