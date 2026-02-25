#pragma once
#include <sqlite3.h>
#include <string>

class Database {
public:
    Database(const std::string& path);
    ~Database();

    bool init();
    bool isEmpty(const std::string& table);
    bool insertPlayer(const std::string& name, const std::string& cls, int level);
    bool insertClass(const std::string& name, const std::string& role, const std::string& specs);

private:
    std::string m_path;
    sqlite3* m_db = nullptr;

    bool exec(const std::string& sql);
};
