#include "player.h"


Stats& Player::getTotalStats(){


    Stats player_stats = {};
    
    for(int i = 0; i < this->items.size(); i++){
        Stats itemStats = items[i]->itemStats;

        player_stats.criticalStrike += itemStats.criticalStrike;

    }
    //Band Aid fix
    return player_stats;  
    
}