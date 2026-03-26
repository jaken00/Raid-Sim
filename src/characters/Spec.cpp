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
void Spec::setSpecFightAffinity(){
    static const FightAffinityProfile allProfiles[] = {
        FightAffinityProfile::single_target_modifier,
        FightAffinityProfile::aoe_modifier,
        FightAffinityProfile::cleave_modifier,
        FightAffinityProfile::movement_modifier,
        FightAffinityProfile::execute_modifier,
        FightAffinityProfile::melee_hostile_penalty,
    };

    for (FightAffinityProfile profile : allProfiles) {
        fightAffinityProfile[profile] = 1.0f;
    }
}


float Spec::getFightAffinity(FightAffinityProfile fightType){
    if(fightAffinityProfile[fightType]){
        return fightAffinityProfile[fightType];
    }
    else {
        return 0;  
    }
}

void Spec::initSpec(){
    setSpecWeights();
    setSpecFightAffinity();
}