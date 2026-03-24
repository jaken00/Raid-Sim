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

void Spec::setFightAffinityProfile(){


}

void Spec::initSpec(){
    setSpecWeights();
    setFightAffinityProfile(0);
}