#include "gamestate.h"
GameState::GameState() {}


PhaseResult GameState::attemptRaid(){
    std::vector<Player*> playerPtrs;

    for(Player& p : players){
        playerPtrs.push_back(&p);
    }
    
    Fight fightAttempt = Fight(playerPtrs, boss);
    PhaseResult fightResults = fightAttempt.attemptPhase();

    return fightResults;

}   



void GameState::runLoader(Database& db){
    Loader loader = Loader();
    players = loader.loadPlayers(db);
    boss = loader.loadBosses(db);
    
}

