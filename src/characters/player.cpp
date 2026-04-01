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
