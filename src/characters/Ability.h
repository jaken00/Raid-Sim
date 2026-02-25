#include "Resource.h"
#include "enum.h"
#include <string>

class Ability
{
private:

public:
    Ability(std::string name, DamageType damageType, CastType castType, Resource resourceUsed);
    ~Ability();
};

Ability::Ability(std::string name, DamageType damageType, CastType castType, Resource resourceUsed)
{
}

Ability::~Ability()
{
}
