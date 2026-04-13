#include "loader.h"

static AttackRange parseAttackRange(std::string s){
    if(s == "Ranged") return AttackRange::Ranged;
    if(s == "Melee") return AttackRange::Melee;
    return AttackRange::Caster;
}

static DamageType parseDamageType(std::string s){
    if(s == "Physical") return DamageType::Physical;
    if(s == "Fire") return DamageType::Fire;
    if(s == "Storm") return DamageType::Storm;
    if(s == "Frost") return DamageType::Frost;
    if(s == "Shadow") return DamageType::Shadow;
    if(s == "Radiant") return DamageType::Radiant;
    return DamageType::Physical;
}

static Role parseRole(std::string s){
    if(s == "Tank")          return Role::Tank;
    if(s == "OffTank")       return Role::OffTank;
    if(s == "Healer")        return Role::Healer;
if(s == "Buffer")        return Role::Buffer;
    return Role::DPS;
}

static Attitude parseAttitude(const std::string& s) {
    if (s == "Positive") return Attitude::Positive;
    if (s == "Toxic")    return Attitude::Toxic;
    return Attitude::Neutral;
}

static Slot parseSlot(const std::string& s) {
    if (s == "Head")      return Slot::Head;
    if (s == "Neck")      return Slot::Neck;
    if (s == "Shoulders") return Slot::Shoulders;
    if (s == "Chest")     return Slot::Chest;
    if (s == "Legs")      return Slot::Legs;
    if (s == "Feet")      return Slot::Feet;
    if (s == "MainHand")  return Slot::MainHand;
    return Slot::OffHand;
}

static Items buildItem(const ItemRow& r) {
    Items item;
    item.name                    = r.name;
    item.itemSlot                = parseSlot(r.slot);
    item.ilvl                    = 0.0f;
    item.itemStats               = {};
    item.itemStats.criticalStrike = r.crit;
    item.itemStats.haste         = r.haste;
    item.itemStats.expertise     = r.expertise;
    item.itemStats.constitution  = r.constitution;
    return item;
}

static Spec buildSpec(const SpecRow& r) {
    Resource res;
    res.resourceName          = r.resource;
    res.maxResource           = 100.0f;
    res.resourceCostPerSecond = 0.0f;
    res.currentResource       = 100.0f;

    RaidBuff buff;
    buff.name     = r.raid_buff;
    buff.buffType = BuffType::Stat_Buff;

    SpecStatWeights weights{ r.stat_crit, r.stat_haste, r.stat_expertise, 0.0f };

    std::map<FightAffinityProfile, float> affinity{
        { FightAffinityProfile::single_target_modifier, r.fap_single_target },
        { FightAffinityProfile::aoe_modifier,           r.fap_aoe           },
        { FightAffinityProfile::cleave_modifier,        r.fap_cleave        },
        { FightAffinityProfile::movement_modifier,      r.fap_movement      },
        { FightAffinityProfile::execute_modifier,       r.fap_execute       },
        { FightAffinityProfile::melee_hostile_penalty,  r.fap_melee_hostile },
    };

    return Spec(r.name, parseRole(r.role), res, parseAttackRange(r.attack_range), r.dps_weight,
                parseDamageType(r.spec_damage_type), r.hps_weight,
                r.defensive_weight, r.utility_weight, r.primary_stat,
                r.can_interrupt, r.can_dispel, r.provides_shield,
                r.provides_external_cd, buff, r.execute_bonus, r.aoe_modifier,
                weights, affinity);
}

std::vector<Player> Loader::loadPlayers(Database& db) {
    // Load all specs into a name -> Spec map
    std::vector<SpecRow> spec_rows;
    db.getAllSpecs(spec_rows);

    std::map<std::string, Spec> spec_map;
    for (const SpecRow& sr : spec_rows)
        spec_map.emplace(sr.name, buildSpec(sr));

    // Load players and build domain objects
    std::vector<PlayerRow> player_rows;
    db.getAllPlayers(player_rows);

    std::vector<Player> players;
    for (int i = 0; i < (int)player_rows.size(); i++) {
        const PlayerRow& row = player_rows[i];

        auto it = spec_map.find(row.spec);
        if (it == spec_map.end()) {
            std::cerr << "Unknown spec '" << row.spec << "' for player " << row.name << ", skipping.\n";
            continue;
        }

        std::vector<Items> items;
        for (const ItemRow& ir : row.items)
            items.push_back(buildItem(ir));

        players.push_back(Player(
            i, row.name, it->second, row.ilvl, items,
            row.performance_rating, row.attendance_percent,
            parseAttitude(row.attitude), row.potential, row.max_hp, row.item_class
        ));
        
    }

    for (Player& player : players) {
        player.setCurrentHealth();

        std::vector<SpellRow> spell_rows;
        db.getSpellsBySpec(player.GetSpec().getName(), spell_rows);
        if (!spell_rows.empty()) {
            std::vector<Spell> spells;
            for (const SpellRow& sr : spell_rows) {
                Spell s;
                s.spell_id          = sr.spell_id;
                s.spell_name        = sr.spell_name;
                s.spec_name         = sr.spec_name;
                s.mana_cost         = sr.mana_cost;
                s.heal_value        = sr.heal_value;
                s.damage_value      = sr.damage_value;
                s.isAoe             = sr.is_aoe;
                s.number_of_targets = sr.number_of_targets;
                s.shield_amount     = sr.shield_amount;
                s.provides_buff     = sr.provides_buff;
                s.is_hot            = sr.is_hot;
                s.cooldown          = sr.cooldown;
                spells.push_back(s);
            }
            player.buildHealerState(spells);
        }
    }

    return players;
}

static FightAffinityProfile fightAffinityProfileMatch(std::string s){
    /*
    enum struct FightAffinityProfile {
     single_target_modifier,  // pure patchwerk DPS
     aoe_modifier,            // 4+ targets
     cleave_modifier,         // 2–3 targets, council/adds
     movement_modifier,       // DoTs and instants love movement
     execute_modifier,        // bonus below 20% boss HP
     melee_hostile_penalty,   // bosses with ground effects, cleave, frontal
};
    */

    if(s == "single_target_modifier") return FightAffinityProfile::single_target_modifier;
    if(s == "aoe_modifier") return FightAffinityProfile::aoe_modifier;
    if(s == "cleave_modifier") return FightAffinityProfile::cleave_modifier;
    if(s == "movement_modifier") return FightAffinityProfile::movement_modifier;
    if(s == "execute_modifier") return FightAffinityProfile::execute_modifier;
    if(s == "melee_hostile_penalty") return FightAffinityProfile::melee_hostile_penalty;
    
    return FightAffinityProfile::MISSING_PROFILE;
    


}



static Phase phaseBuilder(const PhaseRow& pr){
    Phase phase;

    phase.phaseNumber = pr.phase_number;
    phase.hp_start_pct = pr.hp_start_pct;
    phase.hp_end_pct = pr.hp_end_pct;
    phase.is_execute_phase = pr.is_execute_phase;
    
    BossMechanic bossMech;
    bossMech.damageValue = pr.damage_value;
    bossMech.name = pr.mechanic_name;
    bossMech.needsInterrupt = pr.need_interrupt;

    phase.mechanicAssociated = bossMech;


    std::string data = pr.fight_types;
    std::stringstream ss(data);

    std::vector<std::string> fight_types_split;
    std::string token; 

    while(std::getline(ss , token, ',')){
        fight_types_split.push_back(token);
    }

    for(std::string word : fight_types_split){
        phase.fightTypes.push_back(fightAffinityProfileMatch(word));
    }

    return phase;
    

}

Boss Loader::loadBosses(Database& db) {
    BossRow row;
    db.getFirstBoss(row);

    std::map<DamageType, float> resistMap{
        { DamageType::Physical, row.resist_physical },
        { DamageType::Fire,     row.resist_fire     },
        { DamageType::Storm,    row.resist_storm    },
        { DamageType::Frost,    row.resist_frost    },
        { DamageType::Shadow,   row.resist_shadow   },
        { DamageType::Radiant,  row.resist_radiant  },
    };

    std::vector<Phase*> phases;
    for (const PhaseRow& pr : row.phases)
        phases.push_back(new Phase(phaseBuilder(pr)));

    return Boss(
        row.name, 0, nullptr, row.max_hp, row.max_hp,
        row.phase_count, 1, (float)row.tuning_ilvl, phases,
        parseDamageType(row.damage_type),
        resistMap
    );
}
