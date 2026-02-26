#include <string>
#include <vector>

#include "Ability.h"
#include "enum.h"
#include "spec.h"

class CharacterClass {
private:
    /* data */
public:
    CharacterClass(int id, std::string className, Role role, Spec specilizations[]);
    ~CharacterClass();
};

CharacterClass::CharacterClass(int id, std::string className, Role role, Spec specilizations[]) {}

CharacterClass::~CharacterClass() {}
