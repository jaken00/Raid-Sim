#include "Spec.h"

float Spec::getFightAffinity(FightAffinityProfile fightType) {
    auto it = fightAffinityProfile.find(fightType);
    if (it != fightAffinityProfile.end())
        return it->second;
    return 0.0f;
}
