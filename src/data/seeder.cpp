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

        db.insertSpecialization("Warrior", "Tank", "Bulwark", "Resolve", "Melee", 0.35, 0.05, 0.9,
                                0.3, "Constitution", 1, 0, 0, 1, "armor_aura_5", 0.05, 0.75);

        db.insertSpecialization("Warrior", "OffTank", "Vanguard", "Resolve", "Melee", 0.55, 0.0,
                                0.75, 0.2, "Strength", 1, 0, 0, 0, "armor_aura_5", 0.05, 0.9);

        db.insertSpecialization("Conduit", "Healer", "Mender", "Attunement", "Caster", 0.1, 1.0,
                                0.15, 0.4, "Intellect", 0, 1, 1, 1, "magic_resist_5", 0.0, 1.1);

        db.insertSpecialization("Conduit", "SupportHealer", "Siphon", "Mana", "Caster", 0.3, 0.7,
                                0.35, 0.7, "Intellect", 0, 1, 1, 1, "mana_regen_3", 0.05, 1.1);

        db.insertSpecialization("Invoker", "PureDPS", "Arcanist", "Essence", "Caster", 1.0, 0.0,
                                0.15, 0.25, "Intellect", 1, 0, 0, 0, "spell_amp_5", 0.1, 1.15);

        db.insertSpecialization("Invoker", "PureDPS", "Elementalist", "Essence", "Caster", 0.95,
                                0.0, 0.15, 0.3, "Intellect", 1, 0, 0, 0, "elemental_amp_5", 0.1,
                                1.25);

        db.insertSpecialization("Reaver", "SupportDPS", "Exsanguinate", "Momentum", "Melee", 0.75,
                                0.0, 0.3, 0.55, "Agility", 1, 0, 0, 0, "physical_amp_5", 0.2, 0.95);

        db.insertSpecialization("Reaver", "PureDPS", "Striker", "Momentum", "Melee", 1.05, 0.0, 0.2,
                                0.2, "Agility", 1, 0, 0, 0, "bleed_amp_5", 0.25, 0.9);

        db.insertSpecialization("Herald", "SupportDPS", "Warsinger", "Resonance", "Ranged", 0.6,
                                0.0, 0.25, 0.8, "Agility", 1, 0, 1, 1, "haste_aura_5", 0.05, 1.05);

        db.insertSpecialization("Herald", "SupportDPS", "Dirge", "Resonance", "Ranged", 0.55, 0.0,
                                0.25, 0.85, "Agility", 0, 1, 1, 1, "damage_taken_5", 0.1, 1.05);
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
