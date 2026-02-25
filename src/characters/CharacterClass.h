#include "spec.h"
#include "enum.h"
#include <string>


class CharacterClass
{
private:
    /* data */
public:
    CharacterClass(int id, std::string className, Role role, Spec specilizations[]);
    ~CharacterClass();
};

CharacterClass::CharacterClass(int id, std::string className, Role role, Spec specilizations[])
{
}

CharacterClass::~CharacterClass()
{
}
