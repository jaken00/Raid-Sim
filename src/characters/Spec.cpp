#include "Spec.h"


void Spec::setSpecWeights(){
    //decide how we want to do this if its in a massive JSON or seeded in from the start? 
    // FOr now testing we will just set all to .6



    SpecStatWeights statWeights = {
        critStrike: 0.6f,
        haste: 0.6f,
        expertise: 0.6f,
        constitution: 0.6f
    };

    this->specStatWeights = statWeights;
}

/*
struct FightAffinityProfile {
    float single_target_modifier;  // pure patchwerk DPS
    float aoe_modifier;            // 4+ targets
    float cleave_modifier;         // 2–3 targets, council/adds
    float movement_modifier;       // DoTs and instants love movement
    float execute_modifier;        // bonus below 20% boss HP
    float melee_hostile_penalty;   // bosses with ground effects, cleave, frontal
};
*/


void Spec::setPlayerFightAffinityProfile(){
    PlayerFightProfile profile = {
        single_target_modifier: 0.6f,
        aoe_modifier: 0.6f,
        cleave_modifier: 0.6f,
        movement_modifier: 0.6f,
        execute_modifier: 0.4f,
        melee_hostile_penalty: 0.1f
    };

    this->specFightProfile = profile;

}

void Spec::initSpec(){
    setSpecWeights();
    setPlayerFightAffinityProfile();
}