#pragma once
#include "../db/database.h"
#include "../characters/player.h"
#include "../Raid/Boss.h"
#include <vector>
#include <string>
#include <iostream>
#include <sstream>

class Loader {
public:
    std::vector<Player> loadPlayers(Database& db);
    Boss loadBosses(Database& db);
};
