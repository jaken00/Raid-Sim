#pragma once
#include <sqlite3.h>

#include <string>

class Database {
public:
    Database(const std::string& path);
    ~Database();

    bool init();
    bool isEmpty(const std::string& table);
    bool insertPlayer(const std::string& name, const std::string& cls, const std::string& spec,
                      float ilvl, int level);
    bool insertClass(const std::string& name);
    bool insertSpecialization(const std::string& parent_class, const std::string& name,
                              const std::string& role, const std::string& resource,
                              const std::string& attack_range);
    bool insertBoss(const std::string& name, int tuning_ilvl, int hps_threshold, int dps_threshold,
                    int interrupt_coverage_needed, int tank_minimum, int dispel_coverage_needed,
                    bool rewards_physical_buffs, bool punishes_melee_heavy);
private:
    std::string m_path;
    sqlite3* m_db = nullptr;

    bool exec(const std::string& sql);
};
