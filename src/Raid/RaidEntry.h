#pragma once

#include <string>
#include <vector>

#include "../characters/player.h"
#include "Boss.h"
#include "Raid.h"

/* This is the Raid Entry attempt of an Entire Raid. It is made up of a number of EncounterEntires
 */

struct RaidEntry {
    int attemptID;
    const std::string RaidName;
    const std::vector<const Player*> roster;
    bool success;
    const Boss *boss;
    float remaining_boss_hp;
    std::vector<const Player*> deaths;
    float interrupt_coverage_percent;
    float hps_output;
    float total_dps_output;
};
