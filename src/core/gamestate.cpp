#include "gamestate.h"

RaidEntry GameState::attemptRaid(const Raid &raid, std::vector<const Player*> players){

}

static AttackRange parseAttackRange(std::string s){
    if(s == "Ranged") return AttackRange::Ranged;
    if(s == "Melee") return AttackRange::Melee;
    return AttackRange::Caster;
}


void GameState::loadPlayers(Database& db){
    std::vector<PlayerRow> rows;
    std::map<std::string, Spec> spec_mapping;

    db.getAllPlayers(rows);

    /*
        1. Write Conversion functions for all strings to Enums Like above.
        2. 
    */

}