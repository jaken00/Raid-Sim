#include "Boss.h"

Boss::Boss(const std::string& name, int id, float currentHP, float maxHP,
           int phaseCount, int currentPhase, float ilvl_threshhold,
           const std::vector<Phase*> bossPhases, DamageType bossDamagetype,
           std::map<DamageType, float> resistMap,
           float melee_attack_value, float spell_attack_value, float enrage_timer)
    : name(name),
      id(id),
      phaseCount(phaseCount),
      currentPhaseNumber(currentPhase),
      ilvl_threshhold(ilvl_threshhold),
      currentHP(currentHP),
      maxHP(maxHP),
      bossPhases(bossPhases),
      bossDamagetype(bossDamagetype),
      resistMap(resistMap),
      current_phase(bossPhases.empty() ? Phase{} : *bossPhases[0]),
      melee_attack_value(melee_attack_value),
      spell_attack_value(spell_attack_value),
      enrage_timer(enrage_timer) {}

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

void Boss::resetPhase() {
    currentPhaseNumber = 0;
    if (!bossPhases.empty())
        current_phase = *bossPhases[0];
}

void Boss::takeDamage(float damage_value) {
    currentHP -= damage_value;
}

Spell Boss::GetRotationSpell(int index) const {
    auto& spells = current_phase.phase_spells;
    return spells[index % spells.size()];

}
int Boss::GetNextSpellIndex(int current) {
    return(current + 1) % current_phase.phase_spells.size();
}
float Boss::GetCastInterval(int spell_index) {
    Spell spell = GetRotationSpell(spell_index);
    return spell.cooldown;
}