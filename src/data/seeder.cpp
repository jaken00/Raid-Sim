#include "seeder.h"

#include <iostream>

void Seeder::seed(Database& db) {
    if (db.isEmpty("classes")) {
        std::cout << "Seeding classes...\n";
        db.insertClass("Warrior");
        db.insertClass("Conduit");
        db.insertClass("Invoker");
        db.insertClass("Reaver");
        db.insertClass("Herald");
    }

    if (db.isEmpty("specialization")) {
        std::cout << "Seeding specializations...\n";
        db.insertSpecialization("Warrior", "Tank", "Bulwark", "Resolve", "Melee");
        db.insertSpecialization("Warrior", "OffTank", "Vanguard", "Resolve", "Melee");
        db.insertSpecialization("Conduit", "Healer", "Mender", "Attunement", "Caster");
        db.insertSpecialization("Conduit", "SupportHealer", "Siphon", "Mana", "Caster");
        db.insertSpecialization("Invoker", "PureDPS", "Arcanist", "Essence", "Caster");
        db.insertSpecialization("Invoker", "PureDPS", "Elementalist", "Essence", "Caster");
        db.insertSpecialization("Reaver", "SupportDPS", "Exsanguinate", "Momentum", "Melee");
        db.insertSpecialization("Reaver", "PureDPS", "Striker", "Momentum", "Melee");
        db.insertSpecialization("Herald", "SupportDPS", "Warsinger", "Resonance", "Ranged");
        db.insertSpecialization("Herald", "SupportDPS", "Dirge", "Resonance", "Ranged");
    }

    if (db.isEmpty("players")) {
        std::cout << "Seeding players...\n";
        db.insertPlayer("Arthas", "Warrior", "Bulwark", 100.0f, 60);
        db.insertPlayer("Anduin", "Conduit", "Mender", 98.0f, 60);
        db.insertPlayer("Jaina", "Invoker", "Elementalist", 99.0f, 60);
        db.insertPlayer("Sylvanas", "Reaver", "Exsanguinate", 97.0f, 60);
        db.insertPlayer("Malfurion", "Herald", "Warsinger", 96.0f, 60);
        db.insertPlayer("Thrall", "Herald", "Dirge", 95.0f, 60);
        db.insertPlayer("Garrosh", "Warrior", "Vanguard", 94.0f, 60);
        db.insertPlayer("Vol'jin", "Conduit", "Mender", 93.0f, 60);
        db.insertPlayer("Mok'Nathal", "Invoker", "Arcanist", 92.0f, 60);
        db.insertPlayer("Sylvanas", "Reaver", "Striker", 91.0f, 60);
        
    }

    if (db.isEmpty("bosses")) {
        std::cout << "Seeding bosses...\n";
        db.insertBoss("Flamelord", 100, 75000, 75000, 1, 2, 4, false, false);
    }
}
