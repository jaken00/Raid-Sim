#pragma once
#include <string>
#include <vector>
#include "RaidTypes.h"

class Raid {
private:
    PhaseBuilder* buildPhase();
    FightBuilder* buildFight();
    const int NUM_BOSSES = 8;

public:
    Raid() = default;   // single constructor
    ~Raid() = default;  // or remove entirely if unnecessary

    RaidBuilder* buildRaid();
};
