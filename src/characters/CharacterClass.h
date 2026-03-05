#include <string>
#include <vector>

#include "Spec.h"
#include "enum.h"

class CharacterClass {
private:
    /* data */
public:
    CharacterClass(int id, std::string className, Role role, Spec specilizations[]);
    ~CharacterClass();
};

CharacterClass::CharacterClass(int id, std::string className, Role role, Spec specilizations[]) {}

CharacterClass::~CharacterClass() {}
