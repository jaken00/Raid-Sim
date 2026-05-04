#include "gamestate.h"
GameState::GameState() {}


EncounterResult GameState::attemptRaid(){
    std::vector<Player*> playerPtrs;

    for(Player& p : players){
        playerPtrs.push_back(&p);
    }

    Fight fightAttempt = Fight(playerPtrs, boss);
    EncounterResult fightResults = fightAttempt.attemptFight();

    m_sim_history = fightAttempt.getSimHistory();

    return fightResults;

}


void GameState::runLoader(Database& db){
    Loader loader = Loader();
    players = loader.loadPlayers(db);
    boss = loader.loadBosses(db);

}
