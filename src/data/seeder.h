#pragma once
#include "../db/database.h"

class Seeder {
public:
    static void seed(Database& db);

private:
    static void seedSpecs(Database& db);
    static void seedPlayers(Database& db);
    static void seedBosses(Database& db);
};
