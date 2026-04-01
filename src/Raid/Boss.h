#pragma once

#include <string>
#include <vector>
#include <map>


#include "Raid.h"
#include "../enum.h"
#include "../sim/SimTypes.h"


struct BossMechanic {
    std::string name;
    float damageValue;
    bool needsInterrupt;
    

};

struct Phase {
    BossMechanic mechanicAssociated;
    int phaseNumber;
    bool is_execute_phase;
    std::vector<FightAffinityProfile> fightTypes;
    float hp_start_pct;
    float hp_end_pct;
};

class Boss {
private:
    std::string name;
    int id;
    int phaseCount;
    int currentPhaseNumber;
    float ilvl_threshhold;
    Raid* raid;
    float currentHP;
    float maxHP;
    std::vector<Phase*> bossPhases;
    DamageType bossDamagetype;
    std::map<DamageType, float> resistMap; 
    Phase current_phase;

public:
    Boss(const std::string& name, int id, Raid* raid, float currentHP, float maxHP,
         int phaseCount, int currentPhase, float ilvl_threshhold,
         const std::vector<Phase*> bossPhases, DamageType bossDamagetype,
         std::map<DamageType, float> resistMap);
    Boss() : id(0), phaseCount(0),currentPhaseNumber(0),ilvl_threshhold(0), raid(nullptr), currentHP(0), maxHP(0), bossDamagetype(DamageType::Physical) {} //default construor

    ~Boss();

    float getBossResistType(DamageType damageType);
    float GetBossilvl();
    Phase getCurrentPhase();
    float GetCurrentHP();
    void AdvancePhase();
    
    std::string GetName() const;
    float getMaxHP(); 
    int GetPhaseCount();


};

inline float Boss::GetBossilvl(){
    return ilvl_threshhold;
}

inline Phase Boss::getCurrentPhase(){
    return current_phase;
}

inline float Boss::getMaxHP(){
    return maxHP; 
}

inline std::string Boss::GetName() const {
    return name;
}
inline float Boss::GetCurrentHP() {
    return currentHP;
}

inline int Boss::GetPhaseCount(){
    return phaseCount;
}