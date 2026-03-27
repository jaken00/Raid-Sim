#pragma once
#include "../db/database.h"

class Loader {
public:
    static void loadPlayers(Database& db);
    static void loadBosses(Database& db);
};
