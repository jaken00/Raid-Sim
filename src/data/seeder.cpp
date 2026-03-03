#include "seeder.h"

#include <iostream>

void Seeder::seed(Database& db) {
    if (db.isEmpty("classes")) {
        std::cout << "Seeding classes...\n";
        db.insertClass("Warrior", "Bulwark, Vanguard");
        db.insertClass("Conduit", "Mender, Siphon");
        db.insertClass("Invoker", "Elementalist");
        db.insertClass("Reaver", "Exsanguinate, Striker");
        db.insertClass("Herald", "Warsinger, Dirge");
    }

    if (db.isEmpty("specializations")) {
        std::cout << "Seeding Specs\n";
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

    // HERE IS WHERE WE WILL BE ADDING IN OUR PLAYER
    if (db.isEmpty("players")) {
        std::cout << "Seeding players...\n";
        db.insertPlayer("Arthas", "Warrior", 60);
        db.insertPlayer("Anduin", "Priest", 60);
        db.insertPlayer("Jaina", "Mage", 60);
    }
}
