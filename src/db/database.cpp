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
            id      INTEGER PRIMARY KEY AUTOINCREMENT,
            name    TEXT NOT NULL,
            class   TEXT NOT NULL,
            spec    TEXT NOT NULL,
            ilvl    FLOAT NOT NULL DEFAULT 0,
            level   INTEGER NOT NULL DEFAULT 1
        );
    )";

    const std::string create_classes = R"(
        CREATE TABLE IF NOT EXISTS classes (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            name                TEXT NOT NULL
        );
    )";

    const std::string create_specialization = R"(
        CREATE TABLE IF NOT EXISTS specialization (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            parent_class        TEXT NOT NULL,
            role                TEXT NOT NULL,
            name                TEXT NOT NULL,
            resource            TEXT NOT NULL,
            attack_range        TEXT NOT NULL,

            dps_weight          REAL DEFAULT 0.0,
            hps_weight          REAL DEFAULT 0.0,
            defensive_weight    REAL DEFAULT 0.0,
            utility_weight      REAL DEFAULT 0.0,

            -- Primary stat this spec scales with
            primary_stat        TEXT NOT NULL DEFAULT '',

            -- Mechanic flags
            can_interrupt       BOOLEAN DEFAULT 0,
            can_dispel          BOOLEAN DEFAULT 0,
            provides_shield     BOOLEAN DEFAULT 0,
            provides_external_cd BOOLEAN DEFAULT 0,
            raid_buff           TEXT NULL,       -- e.g. "physical_amp_5"
            execute_bonus       REAL DEFAULT 0.0,
            aoe_modifier        REAL DEFAULT 1.0,

            FOREIGN KEY (parent_class) REFERENCES classes(name)
        );

    )";

    const std::string create_bosses = R"(
        CREATE TABLE IF NOT EXISTS bosses (
            id                  INTEGER PRIMARY KEY AUTOINCREMENT,
            name                TEXT NOT NULL,
            tuning_ilvl         INTEGER NOT NULL,
            hps_threshold       INTEGER NOT NULL,
            dps_threshold       INTEGER NOT NULL,
            interrupt_coverage_needed INTEGER NOT NULL,
            tank_minimum        INTEGER NOT NULL,
            dispel_coverage_needed INTEGER NOT NULL,
            rewards_physical_buffs BOOLEAN DEFAULT 0, --need to look for better way of descriping weaknessses
            punishes_melee_heavy   BOOLEAN DEFAULT 0
        );
    )";

    if (!exec(create_players))
        return false;
    if (!exec(create_classes))
        return false;
    if (!exec(create_specialization))
        return false;
    if (!exec(create_bosses))
        return false;

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

bool Database::insertPlayer(const std::string& name, const std::string& cls,
                            const std::string& spec, float ilvl, int level) {
    const char* sql =
        "INSERT INTO players (name, class, spec, ilvl, level) VALUES (?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, cls.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, spec.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_double(stmt, 4, static_cast<double>(ilvl));
    sqlite3_bind_int(stmt, 5, level);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok)
        std::cerr << "Insert player failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::insertClass(const std::string& name) {
    const char* sql = "INSERT INTO classes (name) VALUES (?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok)
        std::cerr << "Insert class failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::insertSpecialization(const std::string& parent_class, const std::string& role,
                                    const std::string& name, const std::string& resource,
                                    const std::string& attack_range) {
    const char* sql =
        "INSERT INTO specialization (parent_class, role, name, resource, attack_range) VALUES (?, "
        "?, ?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, parent_class.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, role.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 4, resource.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 5, attack_range.c_str(), -1, SQLITE_STATIC);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok)
        std::cerr << "Insert class failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::insertBoss(const std::string& name, int tuning_ilvl, int hps_threshold,
                          int dps_threshold, int interrupt_coverage_needed, int tank_minimum,
                          int dispel_coverage_needed, bool rewards_physical_buffs,
                          bool punishes_melee_heavy) {
    const char* sql =
        "INSERT INTO bosses (name, tuning_ilvl, hps_threshold, dps_threshold, "
        "interrupt_coverage_needed, tank_minimum, dispel_coverage_needed, rewards_physical_buffs, "
        "punishes_melee_heavy) VALUES (?, ?, ?, ?, ?, ?, ?, ?, ?);";
    sqlite3_stmt* stmt;
    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }
    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_int(stmt, 2, tuning_ilvl);
    sqlite3_bind_int(stmt, 3, hps_threshold);
    sqlite3_bind_int(stmt, 4, dps_threshold);
    sqlite3_bind_int(stmt, 5, interrupt_coverage_needed);
    sqlite3_bind_int(stmt, 6, tank_minimum);
    sqlite3_bind_int(stmt, 7, dispel_coverage_needed);
    sqlite3_bind_int(stmt, 8, rewards_physical_buffs);
    sqlite3_bind_int(stmt, 9, punishes_melee_heavy);
    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok)
        std::cerr << "Insert boss failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::getFirstPlayer(PlayerRow& out) {
    const char* sql = "SELECT name, class, spec, ilvl, level FROM players ORDER BY id LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (getFirstPlayer): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    bool ok = false;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char* name = sqlite3_column_text(stmt, 0);
        const unsigned char* cls = sqlite3_column_text(stmt, 1);
        const unsigned char* spec = sqlite3_column_text(stmt, 2);

        out.name = name ? reinterpret_cast<const char*>(name) : "";
        out.class_name = cls ? reinterpret_cast<const char*>(cls) : "";
        out.spec = spec ? reinterpret_cast<const char*>(spec) : "";
        out.ilvl = static_cast<float>(sqlite3_column_double(stmt, 3));
        out.level = sqlite3_column_int(stmt, 4);
        ok = true;
    } else if (rc != SQLITE_DONE) {
        std::cerr << "Query failed (getFirstPlayer): " << sqlite3_errmsg(m_db) << "\n";
    }

    sqlite3_finalize(stmt);
    return ok;
}

bool Database::getFirstBoss(BossRow& out) {
    const char* sql =
        "SELECT name, tuning_ilvl, hps_threshold, dps_threshold FROM bosses ORDER BY id "
        "LIMIT 1;";
    sqlite3_stmt* stmt = nullptr;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed (getFirstBoss): " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    bool ok = false;
    int rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        const unsigned char* name = sqlite3_column_text(stmt, 0);

        out.name = name ? reinterpret_cast<const char*>(name) : "";
        out.tuning_ilvl = sqlite3_column_int(stmt, 1);
        out.hps_threshold = sqlite3_column_int(stmt, 2);
        out.dps_threshold = sqlite3_column_int(stmt, 3);
        ok = true;
    } else if (rc != SQLITE_DONE) {
        std::cerr << "Query failed (getFirstBoss): " << sqlite3_errmsg(m_db) << "\n";
    }

    sqlite3_finalize(stmt);
    return ok;
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
