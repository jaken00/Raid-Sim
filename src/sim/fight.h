#pragma once

#include <string>
#include <vector>

#include "../Raid/Boss.h"
#include "../Raid/RaidEntry.h"
#include "characters/player.h"

class Fight {
private:
    std::vector<Player*> players;
    Boss* boss;

public:
    Fight(const std::vector<Player*>& players, Boss* boss);
    ~Fight();
    RaidEntry attemptBoss();
};