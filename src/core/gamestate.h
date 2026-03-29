#pragma once

#include <vector>
#include <map>
#include <iostream>
#include "../Raid/Boss.h"
#include "../Raid/Raid.h"
#include "../raid/RaidEntry.h"
#include "../sim/fight.h"
#include "../db/database.h"
#include "../data/loader.h"

class GameState {
private:
    std::vector<Player> players;
    int ilvl_avg;
    Boss boss;
    Raid raid;
    std::vector<RaidEntry> raid_entries;

public:
    GameState();
    PhaseResult attemptRaid(Raid &raid);
    void runLoader(Database& db);
};

