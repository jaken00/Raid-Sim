#include "player.h"

 int Player::GetTotalCritStrike() const {


    int total_crit = 0;
    
    for(int i = 0; i < this->items.size(); i++){
        Stats itemStats = items[i].itemStats;

        total_crit += itemStats.criticalStrike;

    }
    
    return total_crit;
    
}

int Player::GetTotalHaste() const{
    
    int total_haste = 0;
    
    for(int i = 0; i < this->items.size(); i++){
        Stats itemStats = items[i].itemStats;

        total_haste += itemStats.haste;

    }
    
    return total_haste;
}

int Player::GetTotalExpertise() const{
    
    int total_expertise = 0;
    
    for(int i = 0; i < this->items.size(); i++){
        Stats itemStats = items[i].itemStats;

        total_expertise += itemStats.expertise;

    }
    
    return total_expertise;
}

int Player::GetTotalConstitution() const {
    int total = 0;
    for (const auto& item : items) total += item.itemStats.constitution;
	return total;

}



void Player::setCurrentHealth(){
    currentHealth = maxHp;
}

void Player::takeDamage(float damageValue){
    currentHealth = currentHealth - damageValue;
}

void Player::buildHealerState(const std::vector<Spell>& spells) {
    healerState.healing_spells = spells;
    healerState.max_mana = 10000.0f;
    healerState.current_mana = 10000.0f;
    healerState.mana_regen_per_resolve_phase = 500.0f;
	healerState.healing_priority = spells;
}

