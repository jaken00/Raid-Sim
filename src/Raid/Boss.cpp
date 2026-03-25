#include "Boss.h"




float Boss::getBossResistType(DamageType damageType) {
    if(resistMap[damageType]){
        return resistMap[damageType];
    }
    else {
        return 0.0;
    }

    
}