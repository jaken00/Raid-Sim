#pragma once
#include "../db/database.h"
#include "../characters/player.h"
#include "../Raid/Boss.h"
#include <vector>
#include <string>
#include <iostream>

class Loader {
public:
    std::vector<Player> loadPlayers(Database& db);
    Boss loadBosses(Database& db);
};
