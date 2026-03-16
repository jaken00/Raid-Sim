#pragma once

#include <string>
#include <vector>

#include "../Raid/Boss.h"
#include "../Raid/RaidEntry.h"
#include "../characters/player.h"

class Fight {
private:
    std::vector<Player*> players;
    Boss* boss;

    float getPlayerDPS();
    float getPlayerHPS();
    float getPlayerDefense();
    float getPlayerUtility(); // figure out what this means and what this really does? I think this is imporatnt -> Maybe buffer or utility type? HPS buffer / Def / DPS

public:
    Fight(const std::vector<Player*>& players, Boss* boss);
    ~Fight();
    RaidEntry attemptBoss();
};