#include "seeder.h"

#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

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

// Joins a JSON array of strings into a comma-separated string.
static std::string joinFightTypes(const json& arr) {
    std::string result;
    for (const auto& ft : arr) {
        if (!result.empty()) result += ",";
        result += ft.get<std::string>();
    }
    return result;
}

void Seeder::seedSpecs(Database& db) {
    json specs = loadJson("specs.json");
    if (specs.is_null()) return;

    db.clearTable("specialization");
    db.clearTable("classes");

    std::set<std::string> seenClasses;
    for (auto& s : specs["specializations"]) {
        std::string cls = s["class"];
        if (seenClasses.insert(cls).second)
            db.insertClass(cls);
    }

    for (auto& s : specs["specializations"]) {
        const auto& sw  = s["spec_stat_weight"];
        const auto& fap = s["fight_affinity_profile"];

        db.insertSpecialization(
            s["class"], s["role"], s["name"],
            s["resource"], s["attack_range"],
            s["dps_weight"], s["hps_weight"],
            s["defense_weight"], s["utility_weight"],
            s["primary_stat"],
            s["can_interrupt"] ? 1 : 0,
            s["can_dispel"]    ? 1 : 0,
            s["provides_shield"] ? 1 : 0,
            s["provides_external_cd"] ? 1 : 0,
            s["raid_buff"],
            s["execute_bonus"], s["aoe_modifier"],
            s["spec_damage_type"],
            sw["Haste"].get<double>(),
            sw["CriticalStrike"].get<double>(),
            sw["Expertise"].get<double>(),
            fap["single_target_modifier"].get<double>(),
            fap["aoe_modifier"].get<double>(),
            fap["cleave_modifier"].get<double>(),
            fap["movement_modifier"].get<double>(),
            fap["execute_modifier"].get<double>(),
            fap["melee_hostile_penalty"].get<double>()
        );
    }
}

void Seeder::seedPlayers(Database& db) {
    json players = loadJson("players.json");
    if (players.is_null()) return;

    db.clearTable("player_items");
    db.clearTable("players");

    for (auto& p : players["players"]) {
        int playerId = db.insertPlayer(
            p["name"], p["class"], p["spec"],
            p["item_level"].get<float>(),
            p["performance_rating"].get<float>(),
            p["attendance_percent"].get<float>(),
            p["potential"].get<float>(),
            p["item_class"],
            p["attitude"]
        );

        if (playerId < 0) {
            std::cerr << "Seeder: failed to insert player " << p["name"] << "\n";
            continue;
        }

        for (auto& item : p["items"])
            db.insertPlayerItem(playerId, item["slot"], item["name"]);
    }
}

void Seeder::seedBosses(Database& db) {
    json bosses = loadJson("bosses.json");
    if (bosses.is_null()) return;

    db.clearTable("boss_phases");
    db.clearTable("bosses");

    for (auto& b : bosses["bosses"]) {
        const auto& rp = b["resistance_profile"];

        int bossId = db.insertBoss(
            b["name"],
            b["raid"],
            b["max_hp"].get<float>(),
            b["phase_count"].get<int>(),
            b["tuning_ilvl"].get<int>(),
            b["hps_threshold"].get<int>(),
            b["dps_threshold"].get<int>(),
            b["interrupt_coverage_needed"].get<int>(),
            b["tank_minimum"].get<int>(),
            b["dispel_coverage_needed"].get<int>(),
            b["rewards_physical_buffs"].get<bool>(),
            b["punishes_melee_heavy"].get<bool>(),
            b["damage_type"],
            rp["Physical"].get<double>(),
            rp["Fire"].get<double>(),
            rp["Storm"].get<double>(),
            rp["Frost"].get<double>(),
            rp["Shadow"].get<double>(),
            rp["Radiant"].get<double>()
        );

        if (bossId < 0) {
            std::cerr << "Seeder: failed to insert boss " << b["name"] << "\n";
            continue;
        }

        for (auto& phase : b["phases"]) {
            std::string fightTypesCsv = joinFightTypes(phase["fight_types"]);
            const auto& mech = phase["mechanic"];

            db.insertBossPhase(
                bossId,
                phase["phase_number"].get<int>(),
                phase["hp_start_pct"].get<float>(),
                phase["hp_end_pct"].get<float>(),
                phase["is_execute_phase"].get<bool>(),
                fightTypesCsv,
                mech["name"],
                mech["damage_value"].get<float>(),
                mech["needs_interrupt"].get<bool>()
            );
        }
    }
}

void Seeder::seed(Database& db) {
    std::cout << "Seeding specs from specs.json...\n";
    seedSpecs(db);

    std::cout << "Seeding bosses from bosses.json...\n";
    seedBosses(db);

    std::cout << "Seeding players from players.json...\n";
    seedPlayers(db);
}
