#include "seeder.h"
#include <iostream>

void Seeder::seed(Database& db) {
    if (db.isEmpty("classes")) {
        std::cout << "Seeding classes...\n";
        db.insertClass("Warrior", "Tank, OffTank",       "Bulwark, Vanguard");
        db.insertClass("Conduit", "Healer, SupportDPS",  "Mender, Siphon");
        db.insertClass("Invoker", "DPS",                 "Arcanist, Elementalist");
        db.insertClass("Reaver",  "BufferDPS, DPS",      "Exsanguinate, Striker");
        db.insertClass("Herald",  "Buffer, Debuffer",    "Warsinger, Dirge");
    }
    
    
    // HERE IS WHERE WE WILL BE ADDING IN OUR PLAYER
    if (db.isEmpty("players")) {
        std::cout << "Seeding players...\n";
        db.insertPlayer("Arthas",  "Warrior", 60);
        db.insertPlayer("Anduin",  "Priest",  60);
        db.insertPlayer("Jaina",   "Mage",    60);
    }
}

