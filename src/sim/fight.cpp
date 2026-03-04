#include "fight.h"

Fight::Fight(const std::vector<Player*>& players, Boss* boss) : players(players), boss(boss) {}

Fight::~Fight() {}

RaidEntry Fight::attemptBoss() {
    int avg_ilvl;
    std::vector<std::string> modifers;  // need to change this to an actual modifer class or enum?
}