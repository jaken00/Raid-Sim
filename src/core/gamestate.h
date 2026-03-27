#include <vector>
#include <map>
#include "../Raid/Boss.h"
#include "../Raid/Raid.h"
#include "../raid/RaidEntry.h"
#include "../sim/fight.h"
#include "../db/database.h"

class GameState {
private:
    std::vector<Player> players;
    int ilvl_avg;
    Boss boss;
    Raid raid;
    std::vector<RaidEntry> raid_entries;

public:
    GameState();
    RaidEntry attemptRaid(const Raid &raid, std::vector<const Player*> players);
    void loadPlayers(Database& db);
    void loadBoss(Database& db);
};

