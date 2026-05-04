#pragma once

#include <vector>
#include <map>
#include <iostream>
#include "../Raid/Boss.h"
#include "../Raid/Raid.h"
#include "../sim/fight.h"
#include "../db/database.h"
#include "../data/loader.h"

class GameState {
private:
    std::vector<Player> players;
    int ilvl_avg;
    Boss boss;
    Raid raid;
    std::vector<RaidResult> raid_results;
    std::vector<FightStep> m_sim_history;

public:
    GameState();
    EncounterResult attemptRaid();
    void runLoader(Database& db);

    const std::vector<FightStep>& getSimHistory() const { return m_sim_history; }
    float getBossMaxHP() const { return boss.getMaxHP(); }
};
