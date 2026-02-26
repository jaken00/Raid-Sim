#include <vector>

#include "../characters/CharacterClass.h"
#include "../characters/character.h"

class dataload {
private:
    /* data */
public:
    dataload();
    ~dataload();

    // std::vector<CharacterClass*> loadToons();
    Character* loadCharacter();

    void loadCharacterNeeds(
        Character&);  // Take data from the seeded DBs and load to Character. This means
    // Abilities
    // Specs
    // Characters
    // Stats
    // Items
};

dataload::dataload(/* args */) {}

dataload::~dataload() {}
