#include "player.h"

 int Player::GetTotalCritStrike() const {


    int total_crit = 0;
    
    for(int i = 0; i < this->items.size(); i++){
        Stats itemStats = items[i]->itemStats;

        total_crit += itemStats.criticalStrike;

    }
    
    return total_crit;
    
}