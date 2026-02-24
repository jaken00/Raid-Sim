#include "spec.h"
#include <string>


class CharacterClass
{
private:
    /* data */
public:
    CharacterClass(int id, std::string className, Spec specilizations[]);
    ~CharacterClass();
};

CharacterClass::CharacterClass(int id, std::string className, Spec specilizations[])
{
}

CharacterClass::~CharacterClass()
{
}
