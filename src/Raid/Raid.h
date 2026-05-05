#pragma once
#include <string>
#include <vector>
#include "RaidTypes.h"


class Raid {
private:
	PhaseBuilder* buildPhase();
    FightBuilder* buildFight();

public:
    Raid();
    ~Raid();
	Raid() {};
	
	RaidBuilder* buildRaid();
	
	

};

