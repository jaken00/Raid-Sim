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
            level   INTEGER NOT NULL DEFAULT 1
        );
    )";

    const std::string create_classes = R"(
        CREATE TABLE IF NOT EXISTS classes (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            name TEXT NOT NULL,
            role TEXT NOT NULL,
            specializations TEXT NOT NULL,
            FOREIGN KEY (name) REFERENCES players(class)
        )
    )";

    if (!exec(create_players)) return false;
    if (!exec(create_classes)) return false;

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

bool Database::insertPlayer(const std::string& name, const std::string& cls, int level) {
    const char* sql = "INSERT INTO players (name, class, level) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, cls.c_str(),  -1, SQLITE_STATIC);
    sqlite3_bind_int (stmt, 3, level);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok) std::cerr << "Insert player failed: " << sqlite3_errmsg(m_db) << "\n";
    sqlite3_finalize(stmt);
    return ok;
}

bool Database::insertClass(const std::string& name, const std::string& role, const std::string& specs) {
    const char* sql = "INSERT INTO classes (name, role, specializations) VALUES (?, ?, ?);";
    sqlite3_stmt* stmt;

    if (sqlite3_prepare_v2(m_db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Prepare failed: " << sqlite3_errmsg(m_db) << "\n";
        return false;
    }

    sqlite3_bind_text(stmt, 1, name.c_str(),  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, role.c_str(),  -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 3, specs.c_str(), -1, SQLITE_STATIC);

    bool ok = sqlite3_step(stmt) == SQLITE_DONE;
    if (!ok) std::cerr << "Insert class failed: " << sqlite3_errmsg(m_db) << "\n";
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
