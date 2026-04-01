#include "Boss.h"

Boss::Boss(const std::string& name, int id, Raid* raid, float currentHP, float maxHP,
           int phaseCount, int currentPhase, float ilvl_threshhold,
           const std::vector<Phase*> bossPhases, DamageType bossDamagetype,
           std::map<DamageType, float> resistMap)
    : name(name),
      id(id),
      phaseCount(phaseCount),
      currentPhaseNumber(currentPhase),
      ilvl_threshhold(ilvl_threshhold),
      raid(raid),
      currentHP(currentHP),
      maxHP(maxHP),
      bossPhases(bossPhases),
      bossDamagetype(bossDamagetype),
      resistMap(resistMap),
      current_phase(bossPhases.empty() ? Phase{} : *bossPhases[0])
{}

Boss::~Boss() {}

float Boss::getBossResistType(DamageType damageType) {
    auto it = resistMap.find(damageType);
    if (it != resistMap.end())
        return it->second;
    return 0.0f;
}

void Boss::AdvancePhase(){
    int next_phase_number = current_phase.phaseNumber + 1;
    
    if(next_phase_number > phaseCount){
        return;
    }

    current_phase = *bossPhases[next_phase_number - 1];

    
}