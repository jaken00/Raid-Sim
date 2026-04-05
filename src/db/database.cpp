#include "database.h"

#include <iostream>

Database::Database(const std::string& path) : m_path(path) {}

Database::~Database() {
    if (m_db) {
        sqlite3_close(m_db);
    }
}

bool Database::init() {
    if (sqlite3_open(m_path.c_str(), &m_db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }
    

    const std::string create_players = R"(
        CREATE TABLE IF NOT EXISTS players (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            name                TEXT NOT NULL,
            class               TEXT NOT NULL,
            spec                TEXT NOT NULL,
            ilvl                REAL NOT NULL DEFAULT 0,
            performance_rating  REAL NOT NULL DEFAULT 0.0,
            attendance_percent  REAL NOT NULL DEFAULT 0.0,
            potential           REAL NOT NULL DEFAULT 0.0,
            max_hp              REAL NOT NULL DEFAULT 0.0,
            item_class          TEXT NOT NULL DEFAULT '',
            attitude            TEXT NOT NULL DEFAULT 'Neutral'
        );
    )";

    const std::string create_player_items = R"(
        CREATE TABLE IF NOT EXISTS player_items (
            id           INTEGER PRIMARY KEY AUTOINCREMENT,
            player_id    INTEGER NOT NULL,
            slot         TEXT NOT NULL,
            item_name    TEXT NOT NULL,
            crit         INTEGER NOT NULL DEFAULT 0,
            haste        INTEGER NOT NULL DEFAULT 0,
            expertise    INTEGER NOT NULL DEFAULT 0,
            constitution INTEGER NOT NULL DEFAULT 0,
            FOREIGN KEY (player_id) REFERENCES players(id)
        );
    )";

    const std::string create_classes = R"(
        CREATE TABLE IF NOT EXISTS classes (
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            name    TEXT NOT NULL
        );
    )";

    const std::string create_specialization = R"(
        CREATE TABLE IF NOT EXISTS specialization (
            id                      INTEGER PRIMARY KEY AUTOINCREMENT,
            parent_class            TEXT NOT NULL,
            role                    TEXT NOT NULL,
            name                    TEXT NOT NULL,
            resource                TEXT NOT NULL,
            attack_range            TEXT NOT NULL,

            dps_weight              REAL DEFAULT 0.0,
            hps_weight              REAL DEFAULT 0.0,
            defensive_weight        REAL DEFAULT 0.0,
            utility_weight          REAL DEFAULT 0.0,

            primary_stat            TEXT NOT NULL DEFAULT '',

            can_interrupt           BOOLEAN DEFAULT 0,
            can_dispel              BOOLEAN DEFAULT 0,
            provides_shield         BOOLEAN DEFAULT 0,
            provides_external_cd    BOOLEAN DEFAULT 0,
            raid_buff               TEXT NULL,
            execute_bonus           REAL DEFAULT 0.0,
            aoe_modifier            REAL DEFAULT 1.0,

            spec_damage_type        TEXT NOT NULL DEFAULT 'Physical',
            stat_haste              REAL DEFAULT 0.0,
            stat_crit               REAL DEFAULT 0.0,
            stat_expertise          REAL DEFAULT 0.0,

            fap_single_target       REAL DEFAULT 1.0,
            fap_aoe                 REAL DEFAULT 1.0,
            fap_cleave              REAL DEFAULT 1.0,
            fap_movement            REAL DEFAULT 1.0,
            fap_execute             REAL DEFAULT 1.0,
            fap_melee_hostile       REAL DEFAULT 1.0,

            FOREIGN KEY (parent_class) REFERENCES classes(name)
        );
    )";

    const std::string create_bosses = R"(
        CREATE TABLE IF NOT EXISTS bosses (
            id                          INTEGER PRIMARY KEY AUTOINCREMENT,
            name                        TEXT NOT NULL,
            raid                        TEXT NOT NULL DEFAULT '',
            max_hp                      REAL NOT NULL DEFAULT 0,
            phase_count                 INTEGER NOT NULL DEFAULT 1,
            tuning_ilvl                 INTEGER NOT NULL,
            hps_threshold               INTEGER NOT NULL,
            dps_threshold               INTEGER NOT NULL,
            interrupt_coverage_needed   INTEGER NOT NULL,
            tank_minimum                INTEGER NOT NULL,
            dispel_coverage_needed      INTEGER NOT NULL,
            rewards_physical_buffs      BOOLEAN DEFAULT 0,
            punishes_melee_heavy        BOOLEAN DEFAULT 0,
            damage_type                 TEXT NOT NULL DEFAULT 'Physical',
            resist_physical             REAL DEFAULT 0.0,
            resist_fire                 REAL DEFAULT 0.0,
            resist_storm                REAL DEFAULT 0.0,
            resist_frost                REAL DEFAULT 0.0,
            resist_shadow               REAL DEFAULT 0.0,
            resist_radiant              REAL DEFAULT 0.0
        );
    )";

    const std::string create_boss_phases = R"(
        CREATE TABLE IF NOT EXISTS boss_phases (
            id                          INTEGER PRIMARY KEY AUTOINCREMENT,
            boss_id                     INTEGER NOT NULL,
            phase_number                INTEGER NOT NULL,
            hp_start_pct                REAL NOT NULL,
            hp_end_pct                  REAL NOT NULL,
            is_execute_phase            BOOLEAN DEFAULT 0,
            fight_types                 TEXT NOT NULL DEFAULT '',
            mechanic_name               TEXT NOT NULL DEFAULT '',
            mechanic_damage_value       REAL DEFAULT 0.0,
            mechanic_needs_interrupt    BOOLEAN DEFAULT 0,
            FOREIGN KEY (boss_id) REFERENCES bosses(id)
        );
    )";

    if (!exec(create_players))       return false;
    if (!exec(create_player_items))  return false;
    if (!exec(create_classes))       return false;
    if (!exec(create_specialization)) return false;
    if (!exec(create_bosses))        return false;
    if (!exec(create_boss_phases))   return false;

    std::cout << "Database initialized at: " << m_path << "\n";
    return true;
}

bool Database::isEmpty(const std::string& table) {
    std::string sql = "SELECT COUNT(*) FROM " + table + ";";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql.c_str(), -1, &stmt, nullptr) != SQLITE_OK)
        return true;

    bool empty = true;
    if (sqlite3_step(stmt) == SQLITE_ROW)
        empty = sqlite3_column_int(stmt, 0) == 0;

    sqlite3_finalize(stmt);
    return empty;
}

// Returns the new row id, or -1 on failure.
int Database::insertPlayer(const std::string& name, const std::string& cls,
                           const std::string& spec, float ilvl, float performance_rating,
                           float attendance_percent, float potential, float max_hp,
                           const std::string& item_class, const std::string& attitude) {
    const char* sql =
        "INSERT INTO players "
        "(name, class, spec, ilvl, performance_rating, attendance_percent, potential, max_hp, item_class, attitude) "
        "VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (insertPlayer): " << sqlite3_errmsg(m_db) << "\n";
        return -1;
    }

    sqlite3_bind_text  (stmt, 1, name.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 2, cls.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 3, spec.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 4, static_cast<double>(ilvl));
    sqlite3_bind_double(stmt, 5, static_cast<double>(performance_rating));
    sqlite3_bind_double(stmt, 6, static_cast<double>(attendance_percent));
    sqlite3_bind_double(stmt, 7, static_cast<double>(potential));
    sqlite3_bind_double(stmt, 8, static_cast<double>(max_hp));
    sqlite3_bind_text  (stmt, 9, item_class.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 10, attitude.c_str(),  -1, SQLITE_TRANSIENT);

    int newId = -1;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        newId = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    } else {
        std::cerr << "Insert player failed: " << sqlite3_errmsg(m_db) << "\n";
    }
    sqlite3_finalize(stmt);
    return newId;
}

bool Database::insertPlayerItem(int player_id, const std::string& slot,
                                const std::string& item_name,
                                int crit, int haste, int expertise, int constitution) {
    const char* sql =
        "INSERT INTO player_items (player_id, slot, item_name, crit, haste, expertise, constitution) "
        "VALUES (?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (insertPlayerItem): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_int (stmt, 1, player_id);
    sqlite3_bind_text(stmt, 2, slot.c_str(),      -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, item_name.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int (stmt, 4, crit);
    sqlite3_bind_int (stmt, 5, haste);
    sqlite3_bind_int (stmt, 6, expertise);
    sqlite3_bind_int (stmt, 7, constitution);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok)
        std::cerr << "Insert player item failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::insertClass(const std::string& name) {
    const char* sql = "INSERT INTO classes (name) VALUES (?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (insertClass): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_TRANSIENT);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok)
        std::cerr << "Insert class failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::insertSpecialization(
    const std::string& parent_class, const std::string& role,
    const std::string& name, const std::string& resource,
    const std::string& attack_range, double dps_weight,
    double hps_weight, double defensive_weight, double utility_weight,
    const std::string& primary_stat, int can_interrupt, int can_dispel,
    int provides_shield, int provides_external_cd, const std::string& raid_buff,
    double execute_bonus, double aoe_modifier, const std::string& spec_damage_type,
    double stat_haste, double stat_crit, double stat_expertise,
    double fap_single_target, double fap_aoe, double fap_cleave,
    double fap_movement, double fap_execute, double fap_melee_hostile) {

    const char* sql =
        "INSERT INTO specialization ("
        "parent_class, role, name, resource, attack_range, "
        "dps_weight, hps_weight, defensive_weight, utility_weight, "
        "primary_stat, can_interrupt, can_dispel, provides_shield, provides_external_cd, "
        "raid_buff, execute_bonus, aoe_modifier, "
        "spec_damage_type, stat_haste, stat_crit, stat_expertise, "
        "fap_single_target, fap_aoe, fap_cleave, fap_movement, fap_execute, fap_melee_hostile"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (insertSpecialization): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text  (stmt,  1, parent_class.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt,  2, role.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt,  3, name.c_str(),            -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt,  4, resource.c_str(),        -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt,  5, attack_range.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt,  6, dps_weight);
    sqlite3_bind_double(stmt,  7, hps_weight);
    sqlite3_bind_double(stmt,  8, defensive_weight);
    sqlite3_bind_double(stmt,  9, utility_weight);
    sqlite3_bind_text  (stmt, 10, primary_stat.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_int   (stmt, 11, can_interrupt);
    sqlite3_bind_int   (stmt, 12, can_dispel);
    sqlite3_bind_int   (stmt, 13, provides_shield);
    sqlite3_bind_int   (stmt, 14, provides_external_cd);
    sqlite3_bind_text  (stmt, 15, raid_buff.c_str(),       -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 16, execute_bonus);
    sqlite3_bind_double(stmt, 17, aoe_modifier);
    sqlite3_bind_text  (stmt, 18, spec_damage_type.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 19, stat_haste);
    sqlite3_bind_double(stmt, 20, stat_crit);
    sqlite3_bind_double(stmt, 21, stat_expertise);
    sqlite3_bind_double(stmt, 22, fap_single_target);
    sqlite3_bind_double(stmt, 23, fap_aoe);
    sqlite3_bind_double(stmt, 24, fap_cleave);
    sqlite3_bind_double(stmt, 25, fap_movement);
    sqlite3_bind_double(stmt, 26, fap_execute);
    sqlite3_bind_double(stmt, 27, fap_melee_hostile);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok)
        std::cerr << "Insert specialization failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

// Returns the new row id, or -1 on failure.
int Database::insertBoss(const std::string& name, const std::string& raid, float max_hp,
                         int phase_count, int tuning_ilvl, int hps_threshold, int dps_threshold,
                         int interrupt_coverage_needed, int tank_minimum,
                         int dispel_coverage_needed, bool rewards_physical_buffs,
                         bool punishes_melee_heavy, const std::string& damage_type,
                         double resist_physical, double resist_fire, double resist_storm,
                         double resist_frost, double resist_shadow, double resist_radiant) {
    const char* sql =
        "INSERT INTO bosses ("
        "name, raid, max_hp, phase_count, tuning_ilvl, hps_threshold, dps_threshold, "
        "interrupt_coverage_needed, tank_minimum, dispel_coverage_needed, "
        "rewards_physical_buffs, punishes_melee_heavy, damage_type, "
        "resist_physical, resist_fire, resist_storm, resist_frost, resist_shadow, resist_radiant"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (insertBoss): " << sqlite3_errmsg(m_db) << "\n";
        return -1;
    }

    sqlite3_bind_text  (stmt,  1, name.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt,  2, raid.c_str(),         -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt,  3, static_cast<double>(max_hp));
    sqlite3_bind_int   (stmt,  4, phase_count);
    sqlite3_bind_int   (stmt,  5, tuning_ilvl);
    sqlite3_bind_int   (stmt,  6, hps_threshold);
    sqlite3_bind_int   (stmt,  7, dps_threshold);
    sqlite3_bind_int   (stmt,  8, interrupt_coverage_needed);
    sqlite3_bind_int   (stmt,  9, tank_minimum);
    sqlite3_bind_int   (stmt, 10, dispel_coverage_needed);
    sqlite3_bind_int   (stmt, 11, static_cast<int>(rewards_physical_buffs));
    sqlite3_bind_int   (stmt, 12, static_cast<int>(punishes_melee_heavy));
    sqlite3_bind_text  (stmt, 13, damage_type.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 14, resist_physical);
    sqlite3_bind_double(stmt, 15, resist_fire);
    sqlite3_bind_double(stmt, 16, resist_storm);
    sqlite3_bind_double(stmt, 17, resist_frost);
    sqlite3_bind_double(stmt, 18, resist_shadow);
    sqlite3_bind_double(stmt, 19, resist_radiant);

    int newId = -1;
    if (sqlite3_step(stmt) == SQLITE_DONE) {
        newId = static_cast<int>(sqlite3_last_insert_rowid(m_db));
    } else {
        std::cerr << "Insert boss failed: " << sqlite3_errmsg(m_db) << "\n";
    }
    sqlite3_finalize(stmt);
    return newId;
}

bool Database::insertBossPhase(int boss_id, int phase_number, float hp_start_pct,
                               float hp_end_pct, bool is_execute_phase,
                               const std::string& fight_types_csv,
                               const std::string& mechanic_name, float mechanic_damage_value,
                               bool mechanic_needs_interrupt) {
    const char* sql =
        "INSERT INTO boss_phases ("
        "boss_id, phase_number, hp_start_pct, hp_end_pct, is_execute_phase, "
        "fight_types, mechanic_name, mechanic_damage_value, mechanic_needs_interrupt"
        ") VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";

    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (insertBossPhase): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_int   (stmt, 1, boss_id);
    sqlite3_bind_int   (stmt, 2, phase_number);
    sqlite3_bind_double(stmt, 3, static_cast<double>(hp_start_pct));
    sqlite3_bind_double(stmt, 4, static_cast<double>(hp_end_pct));
    sqlite3_bind_int   (stmt, 5, static_cast<int>(is_execute_phase));
    sqlite3_bind_text  (stmt, 6, fight_types_csv.c_str(),  -1, SQLITE_TRANSIENT);
    sqlite3_bind_text  (stmt, 7, mechanic_name.c_str(),    -1, SQLITE_TRANSIENT);
    sqlite3_bind_double(stmt, 8, static_cast<double>(mechanic_damage_value));
    sqlite3_bind_int   (stmt, 9, static_cast<int>(mechanic_needs_interrupt));

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok)
        std::cerr << "Insert boss phase failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::getAllSpecs(std::vector<SpecRow>& out) {
    const char* sql =
        "SELECT name, resource, attack_range, dps_weight, hps_weight, defensive_weight, utility_weight, "
        "primary_stat, can_interrupt, can_dispel, provides_shield, provides_external_cd, "
        "raid_buff, execute_bonus, aoe_modifier, spec_damage_type, stat_haste, stat_crit, stat_expertise, "
        "fap_single_target, fap_aoe, fap_cleave, fap_movement, fap_execute, fap_melee_hostile, role "
        "FROM specialization ORDER BY name;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (getAllSpecs): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        SpecRow row;
        auto getText = [&](int col) -> std::string {
            const unsigned char* txt = sqlite3_column_text(stmt, col);
            return txt ? reinterpret_cast<const char*>(txt) : "";
        };

        row.name               = getText(0);
        row.resource           = getText(1);
        row.attack_range       = getText(2);
        row.dps_weight         = static_cast<float>(sqlite3_column_double(stmt, 3));
        row.hps_weight         = static_cast<float>(sqlite3_column_double(stmt, 4));
        row.defensive_weight   = static_cast<float>(sqlite3_column_double(stmt, 5));
        row.utility_weight     = static_cast<float>(sqlite3_column_double(stmt, 6));
        row.primary_stat       = getText(7);
        row.can_interrupt      = sqlite3_column_int(stmt, 8) != 0;
        row.can_dispel         = sqlite3_column_int(stmt, 9) != 0;
        row.provides_shield    = sqlite3_column_int(stmt, 10) != 0;
        row.provides_external_cd = sqlite3_column_int(stmt, 11) != 0;
        row.raid_buff          = getText(12);
        row.execute_bonus      = static_cast<float>(sqlite3_column_double(stmt, 13));
        row.aoe_modifier       = static_cast<float>(sqlite3_column_double(stmt, 14));
        row.spec_damage_type   = getText(15);
        row.stat_haste         = static_cast<float>(sqlite3_column_double(stmt, 16));
        row.stat_crit          = static_cast<float>(sqlite3_column_double(stmt, 17));
        row.stat_expertise     = static_cast<float>(sqlite3_column_double(stmt, 18));
        row.fap_single_target  = static_cast<float>(sqlite3_column_double(stmt, 19));
        row.fap_aoe            = static_cast<float>(sqlite3_column_double(stmt, 20));
        row.fap_cleave         = static_cast<float>(sqlite3_column_double(stmt, 21));
        row.fap_movement       = static_cast<float>(sqlite3_column_double(stmt, 22));
        row.fap_execute        = static_cast<float>(sqlite3_column_double(stmt, 23));
        row.fap_melee_hostile  = static_cast<float>(sqlite3_column_double(stmt, 24));
        row.role               = getText(25);

        out.push_back(row);
    }

    sqlite3_finalize(stmt);
    return true;
}

/*
getAllPlayers queries all rows from players, then for each player fetches their items from
player_items and attaches them. Returns false only on a prepare failure.
*/
bool Database::getAllPlayers(std::vector<PlayerRow>& out) {
    const char* sql =
        "SELECT id, name, class, spec, ilvl, performance_rating, "
        "attendance_percent, potential, max_hp, item_class, attitude "
        "FROM players ORDER BY id;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (getAllPlayers): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    const char* items_sql =
        "SELECT slot, item_name, crit, haste, expertise, constitution "
        "FROM player_items WHERE player_id = ? ORDER BY id;";

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        PlayerRow row;
        int player_id = sqlite3_column_int(stmt, 0);

        auto getText = [&](int col) -> std::string {
            const unsigned char* txt = sqlite3_column_text(stmt, col);
            return txt ? reinterpret_cast<const char*>(txt) : "";
        };

        row.name               = getText(1);
        row.class_name         = getText(2);
        row.spec               = getText(3);
        row.ilvl               = static_cast<float>(sqlite3_column_double(stmt, 4));
        row.performance_rating = static_cast<float>(sqlite3_column_double(stmt, 5));
        row.attendance_percent = static_cast<float>(sqlite3_column_double(stmt, 6));
        row.potential          = static_cast<float>(sqlite3_column_double(stmt, 7));
        row.max_hp             = static_cast<float>(sqlite3_column_double(stmt, 8));
        row.item_class         = getText(9);
        row.attitude           = getText(10);

        // Load items for this player
        sqlite3_stmt* item_stmt = nullptr;
        if (sqlite3_prepare_v2(m_db, items_sql, -1, &item_stmt, nullptr) == SQLITE_OK) {
            sqlite3_bind_int(item_stmt, 1, player_id);
            while (sqlite3_step(item_stmt) == SQLITE_ROW) {
                ItemRow item;
                const unsigned char* slot = sqlite3_column_text(item_stmt, 0);
                const unsigned char* name = sqlite3_column_text(item_stmt, 1);
                item.slot         = slot ? reinterpret_cast<const char*>(slot) : "";
                item.name         = name ? reinterpret_cast<const char*>(name) : "";
                item.crit         = sqlite3_column_int(item_stmt, 2);
                item.haste        = sqlite3_column_int(item_stmt, 3);
                item.expertise    = sqlite3_column_int(item_stmt, 4);
                item.constitution = sqlite3_column_int(item_stmt, 5);
                row.items.push_back(item);
            }
            sqlite3_finalize(item_stmt);
        }

        out.push_back(row);
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::getFirstPlayer(PlayerRow& out) {
    const char* sql =
        "SELECT id, name, class, spec, ilvl, performance_rating, "
        "attendance_percent, potential, item_class, attitude "
        "FROM players ORDER BY id LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (getFirstPlayer): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto getText = [&](int col) -> std::string {
            const unsigned char* txt = sqlite3_column_text(stmt, col);
            return txt ? reinterpret_cast<const char*>(txt) : "";
        };

        out.name               = getText(1);
        out.class_name         = getText(2);
        out.spec               = getText(3);
        out.ilvl               = static_cast<float>(sqlite3_column_double(stmt, 4));
        out.performance_rating = static_cast<float>(sqlite3_column_double(stmt, 5));
        out.attendance_percent = static_cast<float>(sqlite3_column_double(stmt, 6));
        out.potential          = static_cast<float>(sqlite3_column_double(stmt, 7));
        out.item_class         = getText(8);
        out.attitude           = getText(9);
        ok = true;
    }

    sqlite3_finalize(stmt);
    return ok;
}

bool Database::getFirstBoss(BossRow& out) {
    const char* sql =
        "SELECT id, name, raid, max_hp, phase_count, tuning_ilvl, hps_threshold, dps_threshold, "
        "damage_type, resist_physical, resist_fire, resist_storm, resist_frost, resist_shadow, resist_radiant "
        "FROM bosses ORDER BY id LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (getFirstBoss): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    bool ok = false;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        auto getText = [&](int col) -> std::string {
            const unsigned char* txt = sqlite3_column_text(stmt, col);
            return txt ? reinterpret_cast<const char*>(txt) : "";
        };

        int boss_id         = sqlite3_column_int(stmt, 0);
        out.name            = getText(1);
        out.raid            = getText(2);
        out.max_hp          = static_cast<float>(sqlite3_column_double(stmt, 3));
        out.phase_count     = sqlite3_column_int(stmt, 4);
        out.tuning_ilvl     = sqlite3_column_int(stmt, 5);
        out.hps_threshold   = sqlite3_column_int(stmt, 6);
        out.dps_threshold   = sqlite3_column_int(stmt, 7);
        out.damage_type     = getText(8);
        out.resist_physical = static_cast<float>(sqlite3_column_double(stmt, 9));
        out.resist_fire     = static_cast<float>(sqlite3_column_double(stmt, 10));
        out.resist_storm    = static_cast<float>(sqlite3_column_double(stmt, 11));
        out.resist_frost    = static_cast<float>(sqlite3_column_double(stmt, 12));
        out.resist_shadow   = static_cast<float>(sqlite3_column_double(stmt, 13));
        out.resist_radiant  = static_cast<float>(sqlite3_column_double(stmt, 14));

        sqlite3_finalize(stmt);
        stmt = nullptr;

        getBossPhases(boss_id, out.phases);
        ok = true;
    } else {
        sqlite3_finalize(stmt);
        stmt = nullptr;
    }

    return ok;
}
int Database::getBossID(const std::string& boss_name) {
    const char* sql =
        "SELECT id FROM bosses "
        "WHERE name = ?;";

    sqlite3_stmt* stmt;
    int boss_id = -1;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) == SQLITE_OK) {

        // Bind parameter (index starts at 1)
        sqlite3_bind_text(stmt, 1, boss_name.c_str(), -1, SQLITE_TRANSIENT);

        if (sqlite3_step(stmt) == SQLITE_ROW) {
            boss_id = sqlite3_column_int(stmt, 0);
        }
    }

    sqlite3_finalize(stmt);
    return boss_id;
}
bool Database::getBossPhases(int boss_id, std::vector<PhaseRow>& out) {
    const char* sql =
        "SELECT phase_number, hp_start_pct, hp_end_pct, is_execute_phase, "
        "fight_types, mechanic_name, mechanic_damage_value, mechanic_needs_interrupt "
        "FROM boss_phases "
        "WHERE boss_id = ? ORDER BY phase_number;";

    sqlite3_stmt* stmt = nullptr;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (getBossPhases): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_int(stmt, 1, boss_id);

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        PhaseRow row;

        auto getText = [&](int col) -> std::string {
            const unsigned char* txt = sqlite3_column_text(stmt, col);
            return txt ? reinterpret_cast<const char*>(txt) : "";
        };

        row.phase_number     = sqlite3_column_int(stmt, 0);
        row.hp_start_pct     = static_cast<float>(sqlite3_column_double(stmt, 1));
        row.hp_end_pct       = static_cast<float>(sqlite3_column_double(stmt, 2));
        row.is_execute_phase = sqlite3_column_int(stmt, 3) != 0;
        row.fight_types      = getText(4);
        row.mechanic_name    = getText(5);
        row.damage_value     = static_cast<float>(sqlite3_column_double(stmt, 6));
        row.need_interrupt   = sqlite3_column_int(stmt, 7) != 0;

        out.push_back(row);
    }

    sqlite3_finalize(stmt);
    return true;
}

bool Database::clearTable(const std::string& table) {
    return exec("DELETE FROM " + table + ";");
}

bool Database::exec(const std::string& sql) {
    char* err = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "SQL error: " << err << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}
