#include <vector>

#include "../Raid/Boss.h"
#include "../Raid/Raid.h"
#include "../characters/CharacterClass.h"
#include "../raid/RaidEntry.h"
#include "../sim/fight.h"

class gamestate {
private:
    std::vector<Player> players;
    int ilvl_avg;
    Boss boss;
    Raid raid;
    std::vector<RaidEntry> raid_entries;

public:
    gamestate(std::vector<Player> players, Boss boss);
    ~gamestate();
};

gamestate::gamestate(std::vector<Player> players, Boss boss) : players(players), boss(boss) {}

gamestate::~gamestate() {}
