#include "seeder.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <set>

using json = nlohmann::json;

#ifndef DATA_DIR
#define DATA_DIR "src/data"
#endif

static json loadJson(const std::string& filename) {
    std::string path = std::string(DATA_DIR) + "/" + filename;
    std::ifstream f(path);
    if (!f.is_open()) {
        std::cerr << "Seeder: failed to open " << path << "\n";
        return nullptr;
    }
    return json::parse(f);
}

void Seeder::seed(Database& db) {
    if (db.isEmpty("classes") || db.isEmpty("specialization")) {
        std::cout << "Seeding classes and specializations from specs.json...\n";
        json specs = loadJson("specs.json");
        if (specs.is_null()) return;

        std::set<std::string> seenClasses;
        for (auto& s : specs["specializations"]) {
            std::string cls = s["class"];
            if (seenClasses.insert(cls).second)
                db.insertClass(cls);
        }

        for (auto& s : specs["specializations"]) {
            db.insertSpecialization(
                s["class"], s["role"], s["name"],
                s["resource"], s["attack_range"],
                s["dps_weight"], s["hps_weight"],
                s["defense_weight"], s["utility_weight"],
                s["primary_stat"],
                s["can_interrupt"] ? 1 : 0,
                s["can_dispel"] ? 1 : 0,
                s["provides_shield"] ? 1 : 0,
                s["provides_external_cd"] ? 1 : 0,
                s["raid_buff"],
                s["execute_bonus"], s["aoe_modifier"]
            );
        }
    }

    if (db.isEmpty("players")) {
        std::cout << "Seeding players from players.json...\n";
        json players = loadJson("players.json");
        if (players.is_null()) return;

        for (auto& p : players["players"]) {
            db.insertPlayer(p["name"], p["class"], p["spec"],
                            p["item_level"].get<float>(), p["level"]);
        }
    }

    if (db.isEmpty("bosses")) {
        std::cout << "Seeding bosses from bosses.json...\n";
        json bosses = loadJson("bosses.json");
        if (bosses.is_null()) return;

        for (auto& b : bosses["bosses"]) {
            db.insertBoss(
                b["name"],
                b["tuning_ilvl"], b["hps_threshold"], b["dps_threshold"],
                b["interrupt_coverage_needed"], b["tank_minimum"],
                b["dispel_coverage_needed"],
                b["rewards_physical_buffs"].get<bool>(),
                b["punishes_melee_heavy"].get<bool>()
            );
        }
    }
}
