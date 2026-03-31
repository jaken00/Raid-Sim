#pragma once
#include <sqlite3.h>

#include <string>
#include <vector>

struct ItemRow {
    std::string slot;
    std::string name;
};

struct PlayerRow {
    std::string name;
    std::string class_name;
    std::string spec;
    std::string item_class;
    std::string attitude;
    float ilvl;
    float performance_rating;
    float attendance_percent;
    float potential;
    std::vector<ItemRow> items;
};

struct SpecRow {
    std::string name;
    std::string resource;
    std::string attack_range;
    float dps_weight;
    float hps_weight;
    float defensive_weight;
    float utility_weight;
    std::string primary_stat;
    bool can_interrupt;
    bool can_dispel;
    bool provides_shield;
    bool provides_external_cd;
    std::string raid_buff;
    float execute_bonus;
    float aoe_modifier;
    std::string spec_damage_type;
    float stat_haste;
    float stat_crit;
    float stat_expertise;
    float fap_single_target;
    float fap_aoe;
    float fap_cleave;
    float fap_movement;
    float fap_execute;
    float fap_melee_hostile;
};

struct PhaseRow {
    int phase_number;
    float hp_start_pct;
    float hp_end_pct;
    bool is_execute_phase;
    std::string fight_types; //NEED TO ADD CONVERSION FUCNTION
    std::string mechanic_name;
    float damage_value;
    bool need_interrupt;
};

struct BossRow {
    std::string name;
    std::string raid;
    std::string damage_type;
    int tuning_ilvl;
    int hps_threshold;
    int dps_threshold;
    int phase_count;
    float max_hp;
    float resist_physical;
    float resist_fire;
    float resist_storm;
    float resist_frost;
    float resist_shadow;
    float resist_radiant;
    std::vector<PhaseRow> phases;
};

class Database {
public:
    Database(const std::string& path);
    ~Database();

    bool init();
    bool isEmpty(const std::string& table);

    // Players
    int  insertPlayer(const std::string& name, const std::string& cls, const std::string& spec,
                      float ilvl, float performance_rating, float attendance_percent,
                      float potential, const std::string& item_class,
                      const std::string& attitude);
    bool insertPlayerItem(int player_id, const std::string& slot, const std::string& item_name);

    // Specializations
    bool insertClass(const std::string& name);
    bool insertSpecialization(const std::string& parent_class, const std::string& role,
                              const std::string& name, const std::string& resource,
                              const std::string& attack_range, double dps_weight,
                              double hps_weight, double defensive_weight, double utility_weight,
                              const std::string& primary_stat, int can_interrupt, int can_dispel,
                              int provides_shield, int provides_external_cd,
                              const std::string& raid_buff, double execute_bonus,
                              double aoe_modifier, const std::string& spec_damage_type,
                              double stat_haste, double stat_crit, double stat_expertise,
                              double fap_single_target, double fap_aoe, double fap_cleave,
                              double fap_movement, double fap_execute, double fap_melee_hostile);

    // Bosses
    int  insertBoss(const std::string& name, const std::string& raid, float max_hp,
                    int phase_count, int tuning_ilvl, int hps_threshold, int dps_threshold,
                    int interrupt_coverage_needed, int tank_minimum, int dispel_coverage_needed,
                    bool rewards_physical_buffs, bool punishes_melee_heavy,
                    const std::string& damage_type,
                    double resist_physical, double resist_fire, double resist_storm,
                    double resist_frost, double resist_shadow, double resist_radiant);
    bool insertBossPhase(int boss_id, int phase_number, float hp_start_pct, float hp_end_pct,
                         bool is_execute_phase, const std::string& fight_types_csv,
                         const std::string& mechanic_name, float mechanic_damage_value,
                         bool mechanic_needs_interrupt);

    // Queries
    bool getAllSpecs(std::vector<SpecRow>& out);
    bool getAllPlayers(std::vector<PlayerRow>& out);
    bool getFirstPlayer(PlayerRow& out);
    bool getFirstBoss(BossRow& out);

    int getBossID(const std::string& boss_name);

private:
    std::string m_path;
    sqlite3* m_db = nullptr;

    bool exec(const std::string& sql);
    bool getBossPhases(int boss_id, std::vector<PhaseRow>& out);
};
