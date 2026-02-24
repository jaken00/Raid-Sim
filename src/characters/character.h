#include <stdlib.h>
#include <string>
#include "CharacterClass.h"
#include "Spec.h"
#include "Talents.h"
#include "Stats.h"

/*
    What will be in a character:
        Name
        Class (Class)
        Spec (Class)
        Level
        CurrentXP
        XPToNextLvl
        Abilities []Abilites
        SkillTree []Talents -> This gives the player the bonuses
        Stats Stats -> have a stats class that holds all of the players primary and secopndary stats
        PlayerItems []Items



*/



class Character
{
private:
    
public:
    Character(std::string name, 
        CharacterClass characterClass, 
        Spec specialization, 
        int level, 
        int skill_points_spent, 
        int currentXP, 
        int XPToLvl);
    ~Character();
};

Character::Character(std::string name, 
        CharacterClass characterClass, 
        Spec specialization, 
        int level, 
        int skill_points_spent, 
        int currentXP, 
        int XPToLvl)
{
}

Character::~Character()
{
}
