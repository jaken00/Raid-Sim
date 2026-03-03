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
            primary_stat        TEXT NOT NULL,

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

    if (!exec(create_players))
        return false;
    if (!exec(create_classes))
        return false;
    if (!exec(create_specialization))
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

bool Database::exec(const std::string& sql) {
    char* err = nullptr;
    if (sqlite3_exec(m_db, sql.c_str(), nullptr, nullptr, &err) != SQLITE_OK) {
        std::cerr << "SQL error: " << err << "\n";
        sqlite3_free(err);
        return false;
    }
    return true;
}
